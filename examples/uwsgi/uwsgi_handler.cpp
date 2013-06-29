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
