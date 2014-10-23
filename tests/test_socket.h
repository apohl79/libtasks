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

#include <queue>
#include <vector>

class echo_handler : public tasks::net_io_task {
   public:
    echo_handler(tasks::net::socket& socket) : net_io_task(socket, EV_READ) {}
    bool handle_event(tasks::worker* worker, int revents);

   private:
    std::queue<std::vector<char> > m_write_queue;
    ssize_t m_write_offset = 0;
};

class test_socket : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(test_socket);
    CPPUNIT_TEST(tcp);
    CPPUNIT_TEST(udp);
    CPPUNIT_TEST(unix);
    CPPUNIT_TEST_SUITE_END();

   public:
    void setUp() {}
    void tearDown() {}

   protected:
    void tcp();
    void udp();
    void unix();
};
