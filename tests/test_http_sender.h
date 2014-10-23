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

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <tasks/dispatcher.h>
#include <tasks/net_io_task.h>
#include <tasks/net/http_sender.h>

#include <mutex>
#include <condition_variable>

class test_handler : public tasks::net::http_response_handler {
   public:
    bool handle_response(std::shared_ptr<tasks::net::http_response> response);
};

class test_http_sender : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(test_http_sender);
    CPPUNIT_TEST(requests);
    CPPUNIT_TEST_SUITE_END();

   public:
    void setUp() {}
    void tearDown() {}

   protected:
    void requests();

   private:
    std::condition_variable m_cond;
    std::mutex m_mutex;
};
