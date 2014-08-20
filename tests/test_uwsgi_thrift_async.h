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

#include <IpService.h> // Thrift generated

#include <tasks/net/uwsgi_thrift_async_handler.h>

using namespace tasks;
using namespace tasks::net;

class ip_service_async1
    : public uwsgi_thrift_async_handler<IpService_lookup_result,
                                        IpService_lookup_args> {
public:
    void service(std::shared_ptr<args_t> args);
    std::string service_name() const { return "lookup"; }
};

class ip_service_async2
    : public uwsgi_thrift_async_handler<IpService_lookup_result,
                                        IpService_lookup_args> {
public:
    void service(std::shared_ptr<args_t> args);
    std::string service_name() const { return "lookup"; }
};


class test_uwsgi_thrift_async : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(test_uwsgi_thrift_async);
    CPPUNIT_TEST(request_finish_in_worker_ctx);
    CPPUNIT_TEST(request_finish_exec);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {}
    void tearDown() {}

protected:
    void request_finish_in_worker_ctx();
    void request_finish_exec();
    void request(net_io_task* srv);
};
