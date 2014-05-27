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

#include <sstream>
#include <cassert>
#include <cstring>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#include <tasks/net/socket.h>
#include <tasks/logging.h>

namespace tasks {
namespace net {

socket::socket(socket_type type) : m_fd(-1), m_type(type) {
    if (UDP == m_type) {
        m_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
        assert(m_fd > 0);
    } else if (TCP != m_type) {
        terr("socket: Invalid socket_type! Using TCP.");
        m_type = TCP;
    }
}

void socket::listen(std::string path, int queue_size) throw(socket_exception) {
    m_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    assert(m_fd > 0);
    if (!m_blocking) {
        if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK)) {
            throw socket_exception("fcntl failed: " + std::string(std::strerror(errno)));
        }
    }
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, path.c_str());
    unlink(addr.sun_path);
    if (::bind(m_fd, (struct sockaddr *) &addr, sizeof(addr.sun_family) + path.length())) {
        throw socket_exception("bind failed: " + std::string(std::strerror(errno)));
    }
    if (::listen(m_fd, queue_size) != 0) {
        throw socket_exception("listen failed: " + std::string(std::strerror(errno)));
    }
}

void socket::listen(int port, std::string ip, int queue_size) throw(socket_exception) {
    if (udp()) {
        throw socket_exception("listen failed: can't be called for UDP sockets");
    }
    bind(port, ip, false /* mark this object as tcp socket */);
    if (::listen(m_fd, queue_size)) {
        throw socket_exception("listen failed: " + std::string(std::strerror(errno)));
    }
}

void socket::bind(int port, std::string ip) throw(socket_exception) {
    bind(port, ip, true /* mark this object as udp socket */);
}

void socket::bind(int port, std::string ip, bool udp) throw(socket_exception) {
    int on = 1;
    m_type = UDP;
    m_fd = ::socket(AF_INET, udp ? SOCK_DGRAM: SOCK_STREAM, 0);
    assert(m_fd > 0);
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on))) {
        throw socket_exception("setsockopt failed: " + std::string(std::strerror(errno)));
    }
    if (!m_blocking) {
        if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK)) {
            throw socket_exception("fcntl failed: " + std::string(std::strerror(errno)));
        }
    }
    init_sockaddr(port, ip);
    if (::bind(m_fd, (struct sockaddr *) m_addr.get(), sizeof(*(m_addr.get())))) {
        throw socket_exception("bind failed: " + std::string(std::strerror(errno)));
    }
}

void socket::init_sockaddr(int port, std::string ip) {
    if (nullptr == m_addr) {
        m_addr = std::make_shared<struct sockaddr_in>();
    }
    bzero(m_addr.get(), sizeof(struct sockaddr_in));
    m_addr->sin_family = AF_INET;
    if (ip.length()) {
        if (inet_pton(AF_INET, ip.c_str(), &(m_addr->sin_addr)) < 1) {
            terr("socket: Invalid ip " << ip << "! Binding to 0.0.0.0!" << std::endl); 
            m_addr->sin_addr.s_addr = INADDR_ANY;
        }
    } else {
        m_addr->sin_addr.s_addr = INADDR_ANY;
    }
    m_addr->sin_port = htons(port);
}

socket socket::accept() throw(socket_exception) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int client = ::accept(m_fd, (struct sockaddr *) &addr, &len);
    if (client < 0) {
        throw socket_exception("accept failed: " + std::string(std::strerror(errno)));
    }
    return socket(client);
}

void socket::connect(std::string path) throw(socket_exception) {
    m_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    assert(m_fd > 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, path.c_str());
    if (::connect(m_fd, (struct sockaddr *) &addr, sizeof(addr.sun_family) + path.length())) {
        throw socket_exception("connect failed: " + std::string(std::strerror(errno)));
    }
    if (!m_blocking) {
        if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK)) {
            throw socket_exception("fcntl failed: " + std::string(std::strerror(errno)));
        }
    }
}

void socket::connect(std::string host, int port) throw(socket_exception) {
    struct hostent* remote = gethostbyname(host.c_str());
    if (nullptr == remote) {
        throw socket_exception("Host " + host + " not found");
    }
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert(m_fd > 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    std::memcpy(&addr.sin_addr, remote->h_addr_list[0], remote->h_length);
    addr.sin_port = htons(port);
    if (::connect(m_fd, (struct sockaddr *) &addr, sizeof(addr))) {
        throw socket_exception("connect failed: " + std::string(std::strerror(errno)));
    }
    if (!m_blocking) {
        if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK)) {
            throw socket_exception("fcntl failed: " + std::string(std::strerror(errno)));
        }
    }
}

void socket::close() {
    if (-1 != m_fd) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void socket::shutdown() {
    ::shutdown(m_fd, SHUT_RDWR);
}
    
std::streamsize socket::write(const char* data, std::size_t len,
                              int port, std::string ip) throw(socket_exception) {
    if (m_fd == -1 && udp()) {
        m_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
        assert(m_fd > 0);
    }
    if (port > -1) {
        init_sockaddr(port, ip);
    }
    const sockaddr* addr = nullptr;
    socklen_t addr_len = 0;
    if (nullptr != m_addr) {
        addr = (const sockaddr*) m_addr.get();
        addr_len = sizeof(*addr);
    }
    ssize_t bytes = sendto(m_fd, data, len, SENDTO_FLAGS, addr, addr_len);
    if (bytes < 0 && errno != EAGAIN) {
        std::stringstream s;
        s << "error writing to client file descriptor " << m_fd << ": " << std::strerror(errno);
        throw socket_exception(s.str());
    }
    return bytes;
}

std::streamsize socket::read(char* data, std::size_t len) throw(socket_exception) {
    sockaddr* addr = nullptr;
    socklen_t addr_len = 0;
    if (udp() && nullptr != m_addr) {
        addr = (sockaddr*) m_addr.get();
        addr_len = sizeof(*addr);
    }
    ssize_t bytes = recvfrom(m_fd, data, len, RECVFROM_FLAGS, addr, &addr_len);
    if (bytes < 0 && errno != EAGAIN) {
        std::stringstream s;
        s << "error reading from client file descriptor " << m_fd << ": " << std::strerror(errno);
        throw socket_exception(s.str());
    } else if (bytes == 0) {
        std::stringstream s;
        s << "client " << m_fd << " disconnected";
        throw socket_exception(s.str());
    }
    return bytes;
}

} // net
} // tasks
