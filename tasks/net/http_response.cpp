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

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <boost/algorithm/string/predicate.hpp>

#include <tasks/logging.h>
#include <tasks/net/http_response.h>
#include <tasks/net/socket.h>

namespace tasks {
namespace net {

void http_response::prepare_data_buffer() {
    // Status line
    m_data_buffer.write("HTTP/1.1 ", 9);
    m_data_buffer.write(m_status.c_str(), m_status.length());
    m_data_buffer.write(CRLF, CRLF_SIZE);
    // Headers
    for (auto kv : m_headers) {
        m_data_buffer.write(kv.first.c_str(), kv.first.length());
        m_data_buffer.write(": ", 2);
        m_data_buffer.write(kv.second.c_str(), kv.second.length());
        m_data_buffer.write(CRLF, CRLF_SIZE);
    }
    std::string ct = "Content-Length: " + std::to_string(m_content_buffer.size());
    m_data_buffer.write(ct.c_str(), ct.length());
    m_data_buffer.write(CRLF, CRLF_SIZE);
    m_data_buffer.write(CRLF, CRLF_SIZE);
}

// We are reading things into the content buffer only.
void http_response::read_data(socket& sock) {
    if (io_state::READY == m_state) {
        m_content_buffer.set_size(READ_BUFFER_SIZE_BLOCK);
        m_state = io_state::READ_DATA;
    }
    if (io_state::DONE != m_state) {
        std::streamsize towrite = 0, bytes = 0;
        do {
            towrite = m_content_buffer.to_write() - 1;
            if (towrite < READ_BUFFER_SIZE_BLOCK - 1) {
                m_content_buffer.set_size(m_content_buffer.buffer_size() + READ_BUFFER_SIZE_BLOCK);
                towrite = m_content_buffer.to_write() - 1;
            }
            bytes = sock.read(m_content_buffer.ptr_write(), towrite);
            if (bytes > 0) {
                m_content_buffer.move_ptr_write(bytes);
                if (io_state::READ_DATA == m_state) {
                    // Terminate the string for parsing
                    *(m_content_buffer.ptr_write()) = 0;
                    parse_data();
                }
                tdbg("http_response: read data successfully, " << bytes << " bytes" << std::endl);
                if (m_content_length == m_content_buffer.offset_write() - m_content_start) {
                    *(m_content_buffer.ptr_write()) = 0;
                    m_content_buffer.set_size(m_content_start + m_content_length);
                    m_content_buffer.move_ptr_read_abs(m_content_start);
                    m_state = io_state::DONE;
                }
            }
        } while (towrite == bytes);
    }
}

void http_response::parse_data() {
    // find the next line break
    char* eol = nullptr;
    do {
        if (*(m_content_buffer.ptr(m_last_line_start)) == '\n') {
            m_last_line_start++;
        }
        eol = std::strstr(m_content_buffer.ptr(m_last_line_start), CRLF);
        if (nullptr != eol) {
            std::size_t len = eol - m_content_buffer.ptr(m_last_line_start);
            if (len) {
                *eol = 0;
                parse_line();
                m_last_line_start += len + 1;
            } else {
                // Second line break means content starts
                if (m_chunked_enc) {
                    throw http_exception("http_response: Chunked transfer encoding needs to be implemented!");
                } else if (!m_content_length_exists) {
                    throw http_exception("http_response: Invalid response: Content-Length header missing");
                }
                m_content_start = m_last_line_start + 1;
                if (*(m_content_buffer.ptr(m_content_start)) == '\n') {
                    m_content_start++;
                }
                tdbg("http_response: Content starts at " << m_content_start << std::endl);
                m_state = io_state::READ_CONTENT;
            }
        }
    } while (nullptr != eol && 0 == m_content_start);
}

void http_response::parse_line() {
    if (0 == m_line_number) {
        parse_status();
    } else {
        parse_header();
    }
    m_line_number++;
}

void http_response::parse_status() {
    // HTTP/#.# ### text
    // Skip the first 5 bytes "HTTP/"
    const char* space = std::strchr(m_content_buffer.ptr(m_last_line_start + 5), ' ');
    m_status = space + 1;
    m_status_code = std::atoi(space + 1);
    tdbg("http_response: Status is " << m_status << std::endl);
    if (m_status_code < 100 || m_status_code > 999) {
        throw http_exception("http_response: Invalid status code " + std::to_string(m_status_code));
    }
}

void http_response::parse_header() {
    char* eq = std::strchr(m_content_buffer.ptr(m_last_line_start), ':');
    if (nullptr != eq) {
        *eq = 0;
        do {
            eq++;
        } while (*eq == ' ');
        auto pair =
            m_headers.insert(std::make_pair(std::string(m_content_buffer.ptr(m_last_line_start)), std::string(eq)));
        if (pair.second) {
            tdbg("http_response: Header: " << pair.first->first << " = " << pair.first->second << std::endl);
            if (boost::iequals(pair.first->first, "Content-Length")) {
                m_content_length = atoi(eq);
                m_content_length_exists = true;
                tdbg("http_response: Setting content length to " << m_content_length << std::endl);
            } else if (boost::iequals(pair.first->first, "Transfer-Encoding")) {
                if (pair.first->second == "chunked") {
                    m_chunked_enc = true;
                }
            }
        }
    } else {
        throw http_exception("http_response: Invalid header: " + std::string(m_content_buffer.ptr(m_last_line_start)));
    }
}

}  // net
}  // tasks
