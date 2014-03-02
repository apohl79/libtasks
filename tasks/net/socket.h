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

#ifdef __linux__
#define SENDTO_FLAGS MSG_NOSIGNAL
#define RECVFROM_FLAGS MSG_DONTWAIT
#else
#define SENDTO_FLAGS 0
#define RECVFROM_FLAGS 0
#endif

namespace tasks {
namespace net {

class socket_exception : public std::exception {
public:
    socket_exception(std::string what) : m_what(what) {}
    const char* what() const noexcept {
        return m_what.c_str();
    }
    
private:
    std::string m_what;
};

class socket {
public:
    socket(int fd = -1) : m_fd(fd) {}
    
    inline int fd() const {
        return m_fd;
    }

    // listen for unix domain sockets
    void listen(std::string path, int queue_size = 128) throw(socket_exception);
    
    // listen for network sockets
    void listen(int port, std::string ip = "", int queue_size = 128) throw(socket_exception);

    socket accept() throw(socket_exception);
    
    // connect a domain socket
    void connect(std::string path) throw(socket_exception);

    // connect via tcp
    void connect(std::string host, int port) throw(socket_exception);

    void shutdown();
    void close();
    
    std::size_t write(const char* data, std::size_t len) throw(socket_exception);
    std::size_t read(char* data, std::size_t len) throw(socket_exception);

private:
    int m_fd = -1;
};

} // net
} // tasks

#endif // _TASKS_SOCKET_H_
