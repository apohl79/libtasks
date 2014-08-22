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

#ifndef _UWSGI_THRIFT_ASYNC_PROCESSOR_H_
#define _UWSGI_THRIFT_ASYNC_PROCESSOR_H_

#include <arpa/inet.h>
#include <boost/shared_ptr.hpp>
#include <thrift/Thrift.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/TApplicationException.h>
#include <unordered_set>
#include <future>

#include <tasks/net/uwsgi_task.h>
#include <tasks/net/uwsgi_thrift_transport.h>
#include <tasks/logging.h>
#include <tasks/exec.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace tasks {
namespace net {

template<class handler_type>
class uwsgi_thrift_async_processor : public tasks::net::uwsgi_task {
public:
    typedef uwsgi_thrift_transport<uwsgi_request> in_transport_type;
    typedef uwsgi_thrift_transport<http_response> out_transport_type;
    typedef TBinaryProtocol protocol_type;

    uwsgi_thrift_async_processor(net::socket& s) : uwsgi_task(s) {}

    virtual ~uwsgi_thrift_async_processor() {}

    virtual bool handle_request() {
        boost::shared_ptr<in_transport_type> in_transport(new in_transport_type(request_p()));
        boost::shared_ptr<out_transport_type> out_transport(new out_transport_type(response_p()));
        boost::shared_ptr<protocol_type> in_protocol(new protocol_type(in_transport));
        boost::shared_ptr<protocol_type> out_protocol(new protocol_type(out_transport));

        // Process message
        worker* worker = worker::get();
        std::shared_ptr<handler_type> handler(new handler_type());
        handler->set_uwsgi_task(this);
        handler->on_finish([this, handler, worker, out_protocol] {
                if (handler->error()) {
                    write_thrift_error(std::string("Handler Error: ") + handler->error_string(),
                                       handler->service_name(), out_protocol);
                } else {
                    // Fill the response back in.
                    out_protocol->writeMessageBegin(handler->service_name(), T_REPLY, m_seqid);
                    handler->result_base().__isset.success = true;
                    handler->result_base().write(out_protocol.get());
                    out_protocol->writeMessageEnd();
                    out_protocol->getTransport()->writeEnd();
                    out_protocol->getTransport()->flush();
                    response().set_status("200 OK");
                }
                response().set_header("Content-Type", "application/x-thrift");
                // Make sure we run in the context of a worker thread
                worker->signal_call([this] (struct ev_loop*) {
                        send_response();
                    });
            });

        try {
            std::string fname;
            TMessageType mtype;
            in_protocol->readMessageBegin(fname, mtype, m_seqid);
            if (mtype != protocol::T_CALL && mtype != protocol::T_ONEWAY) {
                write_thrift_error("invalid message type", handler->service_name(), out_protocol);
                send_thrift_response();
            } else if (fname != handler->service_name()) {
                write_thrift_error("invalid method name", handler->service_name(), out_protocol);
                send_thrift_response();
            } else {
                // read the args from the request
                auto args = std::make_shared<typename handler_type::args_t>();
                args->read(in_protocol.get());
                in_protocol->readMessageEnd();
                in_protocol->getTransport()->readEnd();
                handler->service(args);
            }
        } catch (TException& e) {
            write_thrift_error(std::string("TException: ") + e.what(), handler->service_name(), out_protocol);
            send_thrift_response();
        }

        return true;
    }

    inline void send_thrift_response() {
        response().set_header("Content-Type", "application/x-thrift");
        send_response();
    }

    inline void write_thrift_error(std::string msg, std::string service_name, boost::shared_ptr<protocol_type> out_protocol) {
        response().set_header("X-UWSGI_THRIFT_ASYNC_PROCESSOR_ERROR", msg);
        response().set_status("400 Bad Request");
        TApplicationException ae(msg);
        out_protocol->writeMessageBegin(service_name, T_EXCEPTION, m_seqid);
        ae.write(out_protocol.get());
        out_protocol->writeMessageEnd();
        out_protocol->getTransport()->writeEnd();
        out_protocol->getTransport()->flush();
    }

private:
    int32_t m_seqid = 0;
};

} // net
} // tasks

#endif // _UWSGI_THRIFT_ASYNC_PROCESSOR_H_
