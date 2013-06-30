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

#ifndef _UWSGI_HANDLER_H_
#define _UWSGI_HANDLER_H_

#include <boost/shared_ptr.hpp>
#include <memory>

#include <tasks/net/uwsgi_task.h>
#include <tasks/net/uwsgi_response.h>
#include <tasks/net/uwsgi_thrift_transport.h>

#include <transport/TVirtualTransport.h>
#include <protocol/TBinaryProtocol.h>

#include "stats.h"
#include "test_service.h"

class test_msg;

class uwsgi_handler : public tasks::net::uwsgi_task {
public:
	uwsgi_handler(int s) : uwsgi_task(s) {
		stats::inc_clients();
		// create thrift client
		using namespace tasks::net;
		using namespace apache::thrift::protocol;
		boost::shared_ptr<uwsgi_thrift_transport<uwsgi_response> >
			transport(new uwsgi_thrift_transport<uwsgi_response>(response_p()));
		boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
		m_thrift_client = std::unique_ptr<test_serviceClient>(new test_serviceClient(protocol));
	}

	~uwsgi_handler() {
		stats::dec_clients();
	}

	bool handle_request();

private:
	std::unique_ptr<test_serviceClient> m_thrift_client;
};

#endif // _UWSGI_HANDLER_H_
