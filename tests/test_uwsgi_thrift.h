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

#include <tasks/net/uwsgi_thrift_handler.h>

class ip_service : public tasks::net::uwsgi_thrift_handler<IpServiceIf> {
public:
    void lookup(response_type& result, const int32_t ipv4, const ipv6_type& ipv6);
};

class test_uwsgi_thrift : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(test_uwsgi_thrift);
    CPPUNIT_TEST(request);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}

protected:
    void request();

private:
    std::unique_ptr<tasks::net_io_task> m_srv;
};
