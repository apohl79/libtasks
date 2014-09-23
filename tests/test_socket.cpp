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

#include <tasks/dispatcher.h>
#include <tasks/net_io_task.h>
#include <tasks/worker.h>
#include <tasks/net/acceptor.h>
#include <tasks/logging.h>
#include <string.h>
#include <unistd.h>

#include "test_socket.h"

bool echo_handler::handle_event(tasks::worker* worker, int events) {
    if (events & EV_READ) {
        try {
            std::vector<char> buf(1024);
            std::size_t bytes = socket().read(&buf[0], buf.size());
            buf.resize(bytes);
            m_write_queue.push(std::move(buf));
        } catch (tasks::net::socket_exception& e) {
            return false;
        }
    }
    if (events & EV_WRITE) {
        if (!m_write_queue.empty()) {
            std::vector<char>& buf = m_write_queue.front();
            try {
                std::size_t len = buf.size() - m_write_offset;
                std::size_t bytes = socket().write(&buf[m_write_offset], len);
                if (bytes == len) {
                    // buffer send completely
                    m_write_queue.pop();
                    m_write_offset = 0;
                } else {
                    m_write_offset += bytes;
                }
            } catch (tasks::net::socket_exception& e) {
                return false;
            }
        }
    }
    if (m_write_queue.empty()) {
        set_events(EV_READ);
        update_watcher(worker);
    } else {
        set_events(EV_READ|EV_WRITE);
        update_watcher(worker);
    }
    return true;
}


void test_socket::tcp() {
    int port = 22334;
    
    // create acceptor
    auto srv = new tasks::net::acceptor<echo_handler>(port);
    tasks::dispatcher::instance()->add_event_task(srv);

    // create client
    tasks::net::socket client0;
    client0.set_blocking();
    bool success = true;
    try {
        client0.connect("localhost", port);
    } catch (tasks::net::socket_exception& e) {
        success = false;
    }
    CPPUNIT_ASSERT(success);

    tasks::net::socket client1(client0);
    
    // read/write data    
    std::string data = "test123456789";
    std::streamsize bytes = client1.write(data.c_str(), data.length());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    std::vector<char> buf(1024);
    bytes = client1.read(&buf[0], buf.size());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    CPPUNIT_ASSERT(strncmp(data.c_str(), &buf[0], data.length()) == 0);

    client1.close();

    tasks::dispatcher::instance()->remove_event_task(srv);
}

void test_socket::udp() {
    int port = 22335;
    
    // create srv socket
    tasks::net::socket* srv = new tasks::net::socket(tasks::net::socket_type::UDP);
    srv->set_blocking();
    srv->bind(port);

    // create client socket
    tasks::net::socket clnt(tasks::net::socket_type::UDP);
    clnt.set_blocking();

    // read/write data
    std::string data = "test123456789";
    // clnt -> srv
    std::streamsize bytes = clnt.write(data.c_str(), data.length(), port);
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    std::vector<char> buf0(1024);
    bytes = srv->read(&buf0[0], buf0.size());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    CPPUNIT_ASSERT(strncmp(data.c_str(), &buf0[0], data.length()) == 0);
    // srv -> clnt
    tasks::net::socket srv1(*srv); // copy constructor
    delete srv;
    std::vector<char> buf1(1024);
    bytes = srv1.write(data.c_str(), data.length());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    tasks::net::socket clnt1(clnt); // copy constructor
    bytes = clnt1.read(&buf1[0], buf1.size());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    CPPUNIT_ASSERT(strncmp(data.c_str(), &buf1[0], data.length()) == 0);
    
    clnt.close();
    srv1.close();
}

void test_socket::unix() {
    std::string sockfile = "/tmp/libtasks-test.socket";
    
    // create acceptor
    auto srv = new tasks::net::acceptor<echo_handler>(sockfile);
    tasks::dispatcher::instance()->add_event_task(srv);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // create client
    tasks::net::socket client0;
    client0.set_blocking();
    bool success = true;
    try {
        client0.connect(sockfile);
    } catch (tasks::net::socket_exception& e) {
        terr(e.what()<<std::endl);
        success = false;
    }
    CPPUNIT_ASSERT(success);

    tasks::net::socket client1(client0);
    
    // read/write data    
    std::string data = "test123456789";
    std::streamsize bytes = client1.write(data.c_str(), data.length());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    std::vector<char> buf(1024);
    bytes = client1.read(&buf[0], buf.size());
    CPPUNIT_ASSERT(bytes == static_cast<std::streamsize>(data.length()));
    CPPUNIT_ASSERT(strncmp(data.c_str(), &buf[0], data.length()) == 0);

    client1.close();
    
    tasks::dispatcher::instance()->remove_event_task(srv);

    unlink(sockfile.c_str());
}
