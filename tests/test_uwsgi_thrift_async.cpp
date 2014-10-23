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
#include <csignal>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/THttpClient.h>
#include <thrift/transport/TSocket.h>
#include <boost/shared_ptr.hpp>

#include <tasks/net/acceptor.h>
#include <tasks/net/uwsgi_thrift_async_processor.h>
#include <tasks/logging.h>
#include <tasks/exec.h>

#include "test_uwsgi_thrift_async.h"

std::atomic<bool> g_finished(false);

void ip_service_async1::service(std::shared_ptr<args_t> args) {
    key_value_type kv;
    id_name_type val;
    if (args->ipv4 == 123456789) {
        kv.key.id = 1;
        kv.key.name = "city";
        val.id = 123456;
        val.name = "Berlin";
        kv.values.push_back(val);
        result().key_values.push_back(kv);
        kv.values.clear();

        kv.key.id = 2;
        kv.key.name = "country";
        val.id = 3345677;
        val.name = "Germany";
        kv.values.push_back(val);
        result().key_values.push_back(kv);
        // it could take some time to finish, so sleep a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } else {
        set_error("wrong ip address");
    }
    finish();
}

void ip_service_async2::service(std::shared_ptr<args_t> args) {
    tdbg("ip_service_async2::service(" << this << "): entered" << std::endl);
    // run in a different thread
    tasks::exec([this, args] {
        key_value_type kv;
        id_name_type val;
        if (args->ipv4 == 123456789) {
            kv.key.id = 1;
            kv.key.name = "city";
            val.id = 123456;
            val.name = "Berlin";
            kv.values.push_back(val);
            result().key_values.push_back(kv);
            kv.values.clear();

            kv.key.id = 2;
            kv.key.name = "country";
            val.id = 3345677;
            val.name = "Germany";
            kv.values.push_back(val);
            result().key_values.push_back(kv);
            // it could take some time to finish, so sleep a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            set_error("wrong ip address");
        }
        finish();
        g_finished = true;
        tdbg("ip_service_async2::service(" << this << "): finished" << std::endl);
    });
}

void test_uwsgi_thrift_async::request_finish_in_worker_ctx() {
    m_srv1.reset(new acceptor<uwsgi_thrift_async_processor<ip_service_async1> >(12346));
    tasks::net_io_task::add_task(m_srv1.get());
    request("/test2");
}

void test_uwsgi_thrift_async::request_finish_exec() {
    m_srv2.reset(new acceptor<uwsgi_thrift_async_processor<ip_service_async2> >(12347));
    tasks::net_io_task::add_task(m_srv2.get());
    request("/test3");
}

void test_uwsgi_thrift_async::request_finish_exec_timeout() {
    m_srv3.reset(new acceptor<uwsgi_thrift_async_processor<ip_service_async2> >(12348));
    tasks::net_io_task::add_task(m_srv3.get());

    using namespace apache::thrift::protocol;
    using namespace apache::thrift::transport;
    boost::shared_ptr<THttpClient> transport(new THttpClient("Localhost", 18080, "/test4"));
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    IpServiceClient client(protocol);

    // set a low receive timeout
    struct Socket : THttpClient {
        // can access protected member
        static boost::shared_ptr<TTransport> get(THttpClient* x) { return x->*(&Socket::transport_); }
    };
    boost::shared_ptr<TSocket> tsock = boost::dynamic_pointer_cast<TSocket>(Socket::get(transport.get()));

    tsock->setRecvTimeout(100);
    g_finished = false;
    try {
        transport->open();

        int32_t ip = 123456789;
        ipv6_type ipv6;
        response_type r;
        client.lookup(r, ip, ipv6);

        CPPUNIT_ASSERT_MESSAGE("TTransportException expected", false);
    } catch (TTransportException& e) {
        CPPUNIT_ASSERT_MESSAGE(e.what(), std::string(e.what()).find("timed out") != std::string::npos);
        transport->close();
    }

    while (!g_finished) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    tsock->setRecvTimeout(800);
    g_finished = false;
    try {
        transport->open();

        int32_t ip = 123456789;
        ipv6_type ipv6;
        response_type r;
        client.lookup(r, ip, ipv6);

        CPPUNIT_ASSERT_MESSAGE("TTransportException expected", false);
    } catch (TTransportException& e) {
        CPPUNIT_ASSERT_MESSAGE(e.what(), std::string(e.what()).find("timed out") != std::string::npos);
        transport->close();
    }

    while (!g_finished) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void test_uwsgi_thrift_async::request(std::string url) {
    using namespace apache::thrift::protocol;
    using namespace apache::thrift::transport;
    boost::shared_ptr<THttpClient> transport(new THttpClient("Localhost", 18080, url));
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    IpServiceClient client(protocol);

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
        CPPUNIT_ASSERT_MESSAGE(std::string("TTransportException: ") + e.what(), false);
    }

    try {
        transport->open();

        int32_t ip = 0;
        ipv6_type ipv6;
        response_type r;
        client.lookup(r, ip, ipv6);

        transport->close();

        CPPUNIT_ASSERT_MESSAGE("TTransportException expected", false);
    } catch (TTransportException& e) {
        CPPUNIT_ASSERT(e.what() == std::string("Bad Status: HTTP/1.1"));
    }
}
