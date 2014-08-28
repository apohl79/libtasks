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
    // Notify us when the tasks is finished
    sender->on_finish([this]{ m_cond.notify_one(); });

    // Connect to remote
    bool send_ok = true;
    std::string error;
    try {
        sender->send(std::make_shared<tasks::net::http_request>("localhost", "/", 18080));
    } catch (tasks::net::socket_exception& e) {
        send_ok = false;
        error = e.what();
    }
    CPPUNIT_ASSERT_MESSAGE(std::string("send error: ") + error, send_ok);

    // wait for the response
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock);
    lock.unlock();

    // Check returned data
    CPPUNIT_ASSERT_MESSAGE(std::string("g_status_code = ") + std::to_string(g_status_code), g_status_code == 404);
    CPPUNIT_ASSERT_MESSAGE(std::string("g_content_type = ") + g_content_type, g_content_type == "text/html");

    // Second run
    sender = new tasks::net::http_sender<test_handler>();
    sender->on_finish([this]{ m_cond.notify_one(); });

    send_ok = true;
    error = "";
    try {
        sender->send(std::make_shared<tasks::net::http_request>("localhost", "/", 18080));
    } catch (tasks::net::socket_exception& e) {
        send_ok = false;
        error = e.what();
    }
    CPPUNIT_ASSERT_MESSAGE(std::string("send error: ") + error, send_ok);

    std::unique_lock<std::mutex> lock2(m_mutex);
    m_cond.wait(lock2);
    CPPUNIT_ASSERT_MESSAGE(std::string("g_status_code = ") + std::to_string(g_status_code), g_status_code == 404);
    CPPUNIT_ASSERT_MESSAGE(std::string("g_content_type = ") + g_content_type, g_content_type == "text/html");
}

