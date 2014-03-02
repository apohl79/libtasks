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

#include <tasks/dispatcher.h>
#include <tasks/net/acceptor.h>
#include <tasks/net/uwsgi_thrift_processor.h>

#include "IpService.h" // Thrift generated
#include "ip_service.h"

void ip_service::lookup(response_type& result, const int32_t ipv4, const ipv6_type& ipv6) {
	key_value_type kv;
	id_name_type val;
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
}

int main(int argc, char** argv) {
	using namespace tasks;
	using namespace tasks::net;
	
	acceptor<uwsgi_thrift_processor<IpServiceProcessor /* Thrift generated */,
									ip_service         /* Service handler  */
									> > srv(12345);
	dispatcher::instance()->run(1, &srv);

	return 0;
}
