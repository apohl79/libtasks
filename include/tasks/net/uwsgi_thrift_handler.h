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

#ifndef _UWSGI_THRIFT_HANDLER_H_
#define _UWSGI_THRIFT_HANDLER_H_

#include <tasks/net/uwsgi_task.h>
#include <string>
#include <functional>

namespace tasks {
namespace net {

template<class thrift_interface_type>
class uwsgi_thrift_handler : public thrift_interface_type {
public:
    inline void set_uwsgi_task(uwsgi_task* t) {
        m_uwsgi_task = t;
    }

protected:
    // Provide some wrappers to access the request and response structures
    // from a service call implementation.
    inline uwsgi_request& request() {
        return m_uwsgi_task->request();
    }

    inline uwsgi_request* request_p() {
        return m_uwsgi_task->request_p();
    }

    inline http_response& response() {
        return m_uwsgi_task->response();
    }

    inline http_response* response_p() {
        return m_uwsgi_task->response_p();
    }

private:
    uwsgi_task* m_uwsgi_task = nullptr;
};

} // net
} // tasks

#endif // _UWSGI_THRIFT_HANDLER_H_
