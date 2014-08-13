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

#include <arpa/inet.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/THttpClient.h>
#include <boost/shared_ptr.hpp>

#include <tasks/net/acceptor.h>
#include <tasks/net/uwsgi_thrift_async_processor.h>
#include <tasks/logging.h>

#include "test_uwsgi_thrift_async.h"

void ip_service_async::lookup(response_type& result, const int32_t ipv4, const ipv6_type& ipv6) {
    key_value_type kv;
    id_name_type val;
    if (ipv4 == 123456789) {
        kv.key.id = 1;
        kv.key.name = "city";
        val.id = 123456;
        val.name = "Berlin";
        kv.values.push_back(val);    
        result.key_values.push_back(kv);
        kv.values.clear();
    
        kv.key.id = 2;
        kv.key.name = "country";
        val.id = 3345677;
        val.name = "Germany";
        kv.values.push_back(val);    
        result.key_values.push_back(kv);
        // it could take some time to finish, so sleep a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } else {
        set_error("wrong ip address");
    }
    finish();
}

void test_uwsgi_thrift_async::request() {
    using namespace tasks;
    using namespace tasks::net;
    
    auto srv = new acceptor<uwsgi_thrift_async_processor<IpServiceProcessor /* Thrift generated */,
                                                         ip_service_async   /* Service handler  */
                                                         > > (12345);
    tasks::net_io_task::add_task(srv);

    using namespace apache::thrift::protocol;
    using namespace apache::thrift::transport;
    boost::shared_ptr<THttpClient> transport(new THttpClient("localhost", 18080, "/"));
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    IpServiceClient client(protocol);

    bool success = true;
    try {
        transport->open();

        int32_t ip = 123456789;
        ipv6_type ipv6;
        response_type r;
        client.lookup(r, ip, ipv6);

        CPPUNIT_ASSERT(r.key_values.size() == 2);
        CPPUNIT_ASSERT(r.key_values[0].key.id == 1);
        CPPUNIT_ASSERT(r.key_values[0].key.name == "city");
        CPPUNIT_ASSERT(r.key_values[0].values.size() == 1);
        CPPUNIT_ASSERT(r.key_values[0].values[0].id == 123456);
        CPPUNIT_ASSERT(r.key_values[0].values[0].name == "Berlin");
        CPPUNIT_ASSERT(r.key_values[1].key.id == 2);
        CPPUNIT_ASSERT(r.key_values[1].key.name == "country");
        CPPUNIT_ASSERT(r.key_values[1].values.size() == 1);
        CPPUNIT_ASSERT(r.key_values[1].values[0].id == 3345677);
        CPPUNIT_ASSERT(r.key_values[1].values[0].name == "Germany");

        transport->close();
    } catch (TTransportException& e) {
        success = false;
    }
    CPPUNIT_ASSERT(success);

    try {
        transport->open();

        int32_t ip = 0;
        ipv6_type ipv6;
        response_type r;
        client.lookup(r, ip, ipv6);

        transport->close();
    } catch (TTransportException& e) {
        CPPUNIT_ASSERT(e.what() == std::string("Bad Status: HTTP/1.1"));
        success = false;
    }
    CPPUNIT_ASSERT(!success);

    srv->dispose();
}
