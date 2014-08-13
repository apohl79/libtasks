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
    typedef std::function<void ()> handler_finish_func;

    inline void set_uwsgi_task(uwsgi_task* t) {
        m_uwsgi_task = t;
    }

    inline bool error() const {
        return m_error;
    }

    inline const std::string& error_string() const {
        return m_error_string;
    }

    inline void on_finish(handler_finish_func f) {
        m_finish_func = f;
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

    // A handler can make a request fail by setting an error state. The
    // error string will be returned in an HTTP header.
    inline void set_error(std::string error_string) {
        m_error = true;
        m_error_string = error_string;
    }

    // Async handlers call this method to trigger the processor callback
    inline void finish() {
        m_finish_func();
    }

private:
    uwsgi_task* m_uwsgi_task = nullptr;
    bool m_error = false;
    std::string m_error_string;
    handler_finish_func m_finish_func;
};

} // net
} // tasks

#endif // _UWSGI_THRIFT_HANDLER_H_
