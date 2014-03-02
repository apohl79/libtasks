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


#include "test_http_sender.h"

#include <iostream>

int g_status_code = 0;
std::string g_content_type;

bool test_handler::handle_response(std::shared_ptr<tasks::net::http_response> response) {
    g_status_code = response->status_code();
    g_content_type = response->header("Content-Type");
    return false;
}

void test_http_sender::requests() {
    auto* sender = new tasks::net::http_sender<test_handler>();

    // Connect to remote
    CPPUNIT_ASSERT(sender->send(std::make_shared<tasks::net::http_request>("localhost", "/", 8080)));

    // Notify us when the tasks is finished
    sender->on_finish([this]{ m_cond.notify_one(); });

    // Add io task to the system: send and receive data
    // NOTE: The sender will be deleted and the http_request will be cleared. For that reason
    // we have to create a new sender and a new request object for the second request.
    tasks::net_io_task::add_task(sender);

    // wait for the response
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock);
    lock.unlock();

    // Check returned data
    CPPUNIT_ASSERT(g_status_code == 502);
    CPPUNIT_ASSERT(g_content_type == "text/html");

    // Second run
    sender = new tasks::net::http_sender<test_handler>();
    CPPUNIT_ASSERT(sender->send(std::make_shared<tasks::net::http_request>("localhost", "/", 8080)));
    sender->on_finish([this]{ m_cond.notify_one(); });
    tasks::net_io_task::add_task(sender);
    m_cond.wait(lock);
    CPPUNIT_ASSERT(g_status_code == 502);
    CPPUNIT_ASSERT(g_content_type == "text/html");
}

