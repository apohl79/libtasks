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

#ifndef _TASKS_ACCEPTOR_H_
#define _TASKS_ACCEPTOR_H_

#include <cassert>
#include <vector>

#include <tasks/net_io_task.h>
#include <tasks/dispatcher.h>
#include <tasks/logging.h>
#include <tasks/net/socket.h>

namespace tasks {
namespace net {

// Takes a handler class as argument that needs to take the client socket in its constructor.
// See echo_server example.
template<class T>
class acceptor : public net_io_task {
public:
    acceptor(int port) : net_io_task(-1, EV_READ) {
        // Create a non-blocking master socket.
        tdbg("acceptor: listening on port " << port << std::endl);
        socket sock;
        try {
            sock.listen(port);
            set_fd(sock.fd());
        } catch (socket_exception e) {
            terr("acceptor: " << e.what() << std::endl);
            assert(false);
        }
    }

    acceptor(std::string path) : net_io_task(-1, EV_READ) {
        // Create a non-blocking master socket.
        tdbg("acceptor: listening on unix:" << path << std::endl);
        socket sock;
        try {
            sock.listen(path);
            set_fd(sock.fd());
        } catch (socket_exception e) {
            terr("acceptor: " << e.what() << std::endl);
            assert(false);
        }
    }
        
    ~acceptor() {
        socket(fd()).shutdown();
    }

    bool handle_event(worker* worker, int revents)  {
        socket sock(fd());
        try {
            socket client = sock.accept();
            tdbg("acceptor: new client fd " << client.fd() << std::endl);
            T* task = new T(client.fd());
            add_task(worker, task);
        } catch (socket_exception e) {
            terr("acceptor: " << e.what() << std::endl);
            assert(false);
        }
        return true;
    }
};

} // net
} // tasks

#endif // _TASKS_ACCEPTOR_H_
