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

#ifndef _TASKS_SOCKET_H_
#define _TASKS_SOCKET_H_

#include <string>
#include <exception>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>

#include <tasks/tasks_exception.h>

#ifdef _OS_LINUX_
#define SEND_RECV_FLAGS MSG_NOSIGNAL
#else
#define SEND_RECV_FLAGS 0
#endif

struct sockaddr_in;

namespace tasks {
namespace net {

class socket_exception : public tasks::tasks_exception {
public:
    socket_exception(std::string what) : tasks::tasks_exception(what) {}
};

enum class socket_type {
    TCP,
    UDP
};

class socket {
public:
    socket(int fd) : m_fd(fd) {}
    socket(socket_type = socket_type::TCP);
    
    inline int fd() const {
        return m_fd;
    }

    inline bool udp() const {
        return m_type == socket_type::UDP;
    }

    inline bool tcp() const {
        return m_type == socket_type::TCP;
    }

    inline socket_type type() const {
        return m_type;
    }

    inline std::shared_ptr<struct sockaddr_in> addr() {
        return m_addr;
    }

    inline void set_blocking() {
        m_blocking = true;
    }

    // bind for udp sockets. This method can be used to bind udp sockets. For tcp servers
    // socket::listen has to be called.
    void bind(int port, std::string ip = "");

    // listen for unix domain sockets
    void listen(std::string path, int queue_size = 128);
    
    // listen for tcp sockets
    void listen(int port, std::string ip = "", int queue_size = 128);

    socket accept();
    
    // connect a domain socket
    void connect(std::string path);

    // connect via tcp
    void connect(std::string host, int port);

    void shutdown();
    void close();
    
    std::streamsize write(const char* data, std::size_t len,
                          int port = -1, std::string ip = "");
    std::streamsize read(char* data, std::size_t len);

private:
    int m_fd = -1;
    socket_type m_type = socket_type::TCP;
    bool m_blocking = false;
    std::shared_ptr<struct sockaddr_in> m_addr;

    void bind(int port, std::string ip, bool udp);
    void init_sockaddr(int port, std::string ip = "");

};

} // net
} // tasks

#endif // _TASKS_SOCKET_H_
