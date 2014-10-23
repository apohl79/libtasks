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

#ifndef _UWSGI_TASK_H_
#define _UWSGI_TASK_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>

#include <tasks/worker.h>
#include <tasks/net_io_task.h>
#include <tasks/net/uwsgi_request.h>
#include <tasks/net/http_response.h>

// The nginx uwsgi module does not support keepalive connections. If
// this gets implemented one day or we find a different webserver that
// supports this, we can enable the following flag.
#define UWSGI_KEEPALIVE 0

namespace tasks {
namespace net {

class uwsgi_task : public tasks::net_io_task {
   public:
    uwsgi_task(net::socket& sock) : tasks::net_io_task(sock, EV_READ) {}
    virtual ~uwsgi_task() {}

    bool handle_event(tasks::worker* worker, int revents);

    // A request handler needs to implement this
    virtual bool handle_request() = 0;

    inline uwsgi_request& request() { return m_request; }

    inline uwsgi_request* request_p() { return &m_request; }

    inline http_response& response() { return m_response; }

    inline http_response* response_p() { return &m_response; }

    inline void send_response() {
        worker* w = worker::get();
        assert(nullptr != w);
        set_events(EV_WRITE);
        update_watcher(w);
    }

   protected:
    uwsgi_request m_request;
    http_response m_response;

    inline void finish_request() { m_response.clear(); }
};

}  // net
}  // tasks

#endif  // _UWSGI_TASK_H_
