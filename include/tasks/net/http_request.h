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

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <tasks/net/http_base.h>

namespace tasks {
namespace net {

class http_request : public http_base {
   public:
    http_request(std::string host, std::string url = "", int port = 80) : m_url(url), m_port(port) {
        m_headers["Host"] = host;
        init_default_headers();
    }

    inline void set_url(std::string url) { m_url = url; }

    inline int port() const { return m_port; }

    void prepare_data_buffer();

    void clear() {
        http_base::clear();
        m_url = "";
        init_default_headers();
    }

   private:
    std::string m_url;
    int m_port;

    void init_default_headers();
};

}  // net
}  // tasks

#endif  // _HTTP_REQUEST_H_
