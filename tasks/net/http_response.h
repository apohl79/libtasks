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

#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

#include <tasks/net/io_state.h>
#include <tasks/tools/buffer.h>
#include <tasks/logging.h>

#define CRLF "\r\n"
#define CRLF_SIZE 2

#ifdef __linux__
#define SENDTO_FLAGS MSG_NOSIGNAL
#else
#define SENDTO_FLAGS 0
#endif

namespace tasks {
namespace net {

class http_response {
public:
    http_response() {}

    inline void set_status(std::string status) {
        m_status = status;
    }
    
    inline void set_header(std::string header, std::string value) {
        m_headers[header] = value;
    }

    inline void append(std::string s) {
        m_content_buffer.append(s.c_str(), s.length());
    }

    inline void append(const void* data, std::size_t size) {
        m_content_buffer.append(data, size);
    }

    inline std::size_t copy(void *dst, std::size_t size) {
        terr("http_response::copy not supported" << std::endl);
        return -1;
    }

    bool write_data(int fd);

    inline void print() const {
        std::cout << std::string(m_data_buffer.pointer(0), m_data_buffer.size());
        if (m_content_buffer.size()) {
            std::cout << std::string(m_content_buffer.pointer(0), m_content_buffer.size());
        }
    }

    inline bool done() const {
        return m_state == DONE;
    }
    
    inline void clear() {
        m_data_buffer.clear();
        m_content_buffer.clear();
        m_status = "";
        if (m_headers.size() > 0) {
            m_headers.clear();
        }
        m_state = READY;
    }

private:
    tasks::tools::buffer m_data_buffer;
    tasks::tools::buffer m_content_buffer;
    io_state m_state = READY;

    std::string m_status;
    std::unordered_map<std::string, std::string> m_headers;
};

} // net
} // tasks


#endif // _HTTP_RESPONSE_H_
