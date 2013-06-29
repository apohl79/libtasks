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

#include <tasks/dispatcher.h>
#include <tasks/net/acceptor.h>

#ifdef PROFILER
#include <google/profiler.h>
#endif

#include "uwsgi_handler.h"
#include "stats.h"

bool uwsgi_handler::handle_request() {
	// Do something with the url for example
	//const std::string& url = request().var("REQUEST_URI");
	// Or print all incoming variables
	//request().print_vars();

	// Now send back a response
	response().set_status("204 No Content");
	send_response();

	stats::inc_req();
	
	return true;
}

int main(int argc, char** argv) {
#ifdef PROFILER
	ProfilerStart("uwsgi_server.prof");
#endif
	stats s;
	tasks::net::acceptor<uwsgi_handler> srv(12345);
	tasks::dispatcher::instance()->run(2, &srv, &s);
#ifdef PROFILER
	ProfilerStop();
#endif
	return 0;
}
