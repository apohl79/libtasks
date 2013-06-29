#ifndef _UWSGI_HANDLER_H_
#define _UWSGI_HANDLER_H_

#include <tasks/net/uwsgi_task.h>

#include "stats.h"

class uwsgi_handler : public tasks::net::uwsgi_task {
public:
	uwsgi_handler(int s) : uwsgi_task(s) {
		stats::inc_clients();
	}

	~uwsgi_handler() {
		stats::dec_clients();
	}
	
	bool handle_request();
};

#endif // _UWSGI_HANDLER_H_
