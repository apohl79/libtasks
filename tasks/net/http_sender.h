/*
 * Copyright (c) 2013 Andreas Pohl <apohl79 at gmail.com>
 *
 * This file is part of libtasks.
 * 
 * libtasks is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libtasks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with libtasks.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HTTP_SENDER_H_
#define _HTTP_SENDER_H_

#include <memory>
#include <cassert>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <tasks/dispatcher.h>
#include <tasks/logging.h>
#include <tasks/io_task.h>
#include <tasks/net/http_request.h>
#include <tasks/net/http_response.h>

namespace tasks {
namespace net {

class http_response_handler {
public:
    virtual bool handle_response(std::shared_ptr<http_response> response) = 0;
};

template<class handler_type>
class http_sender : public io_task {
public:
    http_sender()
        : io_task(-1, EV_UNDEF), m_response(new http_response()) {}

    http_sender(std::shared_ptr<handler_type> handler)
        : handler_type(), m_handler(handler) {}

    bool handle_event(tasks::worker* worker, int revents) {
        bool success = true;
        if (EV_READ & revents) {
            if (m_response->read_data(fd())) {
                if (m_response->done()) {
                    if (nullptr == m_handler) {
                        m_handler = std::make_shared<handler_type>();
                    }
                    success = m_handler->handle_response(m_response);
                    m_response->clear();
                    m_request->clear();
                    // Stop watching for events until the next send() call
                    //set_events(EV_UNDEF);
                    //update_watcher(worker);
                }
            } else {
                success = false;
            }
        } else if (EV_WRITE & revents) {
            if (m_request->write_data(fd())) {
                if (m_request->done()) {
                    set_events(EV_READ);
                    update_watcher(worker);
                }
            } else {
                success = false;
            }
        }
        return success;
    }

    inline bool send(std::shared_ptr<http_request> request) {
        m_request = request;
        const std::string& host = m_request->header("Host");
        if (-1 == fd() || m_host != host) {
            m_host = host;
            // Close an existing connection
            if (-1 != fd()) {
                close(fd());
            }
            // Connect
            tdbg("http_sender: Resolving " << m_host << std::endl);
            struct hostent* remote = gethostbyname(m_host.c_str());
            if (nullptr == remote) {
                terr("http_sender: Host " << m_host << " not found" << std::endl);
                return false;
            }
            int fd = socket(PF_INET, SOCK_STREAM, 0);
            assert(fd > 0);
            //assert(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == 0);
            struct sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            std::memcpy(&addr.sin_addr, remote->h_addr_list[0], remote->h_length);
            addr.sin_port = htons(m_port);
            tdbg("http_sender: Connecting " << m_host << ":" << m_port << std::endl);
            if (connect(fd, (struct sockaddr *) &addr, sizeof(addr))) {
                terr("http_sender: Connection to " << m_host << ":" << m_port << " failed"
                     << std::endl);
                return false;
            }
            tdbg("http_sender: Connection to " << m_host << ":" << m_port << " successful"
                 << std::endl);
            set_fd(fd);
        }
        m_request->set_header("Host", m_host);
        set_events(EV_WRITE);
        update_watcher(tasks::dispatcher::instance()->first_worker());
        return true;
    }
    
private:
    std::shared_ptr<http_request> m_request;
    std::shared_ptr<http_response> m_response;
    std::shared_ptr<handler_type> m_handler;
    std::string m_host;
    int m_port = 80;
};

} // net
} // tasks

#endif // _HTTP_SENDER_H_
