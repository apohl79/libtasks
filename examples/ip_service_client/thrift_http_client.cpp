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

#include <iostream>
#include <arpa/inet.h>
#include <protocol/TBinaryProtocol.h>
#include <transport/THttpClient.h>
#include <boost/shared_ptr.hpp>
#include "IpService.h"


int main(int argc, char** argv) {
	using namespace apache::thrift::protocol;
	using namespace apache::thrift::transport;
	boost::shared_ptr<THttpClient> transport(new THttpClient("localhost", 8080, "/"));
	boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
	IpServiceClient client(protocol);

	try {
		transport->open();

		std::cout << "Sending request" << std::endl;
		int32_t ip = 123456789;
		ipv6_type ipv6;
		response_type r;
		client.lookup(r, ip, ipv6);

		for (auto& kv : r.key_values) {
			std::cout << "key.id=" << kv.key.id << " key.name=" << kv.key.name << std::endl;
			for (auto& val : kv.values) {
				std::cout << "  val.id=" << val.id << " val.name=" << val.name << std::endl;
			}
		}

		transport->close();
	} catch (TTransportException& e) {
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
