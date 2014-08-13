/*
 * Copyright (c) 2013-2014 Andreas Pohl <apohl79 at gmail.com>
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

#include <tasks/dispatcher.h>
#include <tasks/logging.h>
#include <tasks/net_io_task.h>
#include <tasks/net/http_request.h>
#include <tasks/net/http_response.h>
#include <tasks/net/socket.h>

namespace tasks {
namespace net {

class http_response_handler {
public:
    virtual bool handle_response(std::shared_ptr<http_response> response) = 0;
};

template<class handler_type>
class http_sender : public net_io_task {
public:
    http_sender()
        : net_io_task(EV_UNDEF), m_response(new http_response()) {}

    http_sender(std::shared_ptr<handler_type> handler)
        : net_io_task(EV_UNDEF), handler_type(), m_handler(handler) {}

    bool handle_event(tasks::worker* worker, int revents) {
        bool success = true;
        if (EV_READ & revents) {
            if (m_response->read_data(socket())) {
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
            if (m_request->write_data(socket())) {
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
        if (-1 == socket().fd() || m_host != host) {
            m_host = host;
            m_port = m_request->port();
            // Close an existing connection
            socket().close();
            // Connect
            tdbg("http_sender: Connecting " << m_host << ":" << m_port << std::endl);
            try {
                socket().connect(m_host, m_port);
            } catch (socket_exception e) {
                terr("http_sender: " << e.what() << std::endl);
                return false;
            }
        }
        m_request->set_header("Host", m_host);
        set_events(EV_WRITE);
        update_watcher(tasks::dispatcher::instance()->last_worker());
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
