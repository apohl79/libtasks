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

#include <tasks/logging.h>
#include <sys/socket.h>
#include <cstdlib>

#include <tasks/net/uwsgi_request.h>
#include <tasks/net/socket.h>

namespace tasks {
namespace net {

std::string uwsgi_request::NO_VAL;

void uwsgi_request::read_header(socket& sock) {
    std::streamsize bytes = sock.read((char*) &m_header, sizeof(m_header));
    if (bytes != sizeof(m_header)) {
        throw uwsgi_exception("uwsgi_request: error reading header");
    }
    tdbg("uwsgi_request::read_header: read header successfully, " << bytes << " bytes" << std::endl);
}

void uwsgi_request::read_vars(socket& sock) {
    std::streamsize bytes = sock.read(m_data_buffer.ptr_write(), m_data_buffer.to_write());
    if (bytes > 0) {
        m_data_buffer.move_ptr_write(bytes);
        tdbg("uwsgi_request::read_vars: read data successfully, " << bytes << " bytes" << std::endl);
    }
    if (!m_data_buffer.to_write()) {
        if (UWSGI_VARS == m_header.modifier1) {
            parse_vars();
            // Check if a http body needs to be read
            std::string content_len_s = var("CONTENT_LENGTH");
            if (NO_VAL != content_len_s) {
                std::size_t content_len_i = std::atoi(content_len_s.c_str());
                m_content_buffer.set_size(content_len_i);
                m_state = io_state::READ_CONTENT;
            } else {
                m_state = io_state::DONE;
            }
        }
    }
}

void uwsgi_request::read_content(socket& sock) {
    std::streamsize bytes = sock.read(m_content_buffer.ptr_write(), m_content_buffer.to_write());
    if (bytes > 0) {
        m_content_buffer.move_ptr_write(bytes);
        tdbg("uwsgi_request::read_content: read data successfully, " << bytes << " bytes" << std::endl);
    }
    if (!m_content_buffer.to_write()) {
        m_state = io_state::DONE;
    }
}

void uwsgi_request::read_data(socket& sock) {
    if (io_state::READY == m_state) {
        m_state = io_state::READ_HEADER;
        read_header(sock);
        m_state = io_state::READ_DATA;
        m_data_buffer.set_size(m_header.datasize);
    }
    if (io_state::READ_DATA == m_state) {
        read_vars(sock);
    }
    if (io_state::READ_CONTENT == m_state) {
        read_content(sock);
    }
}

void uwsgi_request::parse_vars() {
    std::size_t pos = 0;
    while (pos < m_data_buffer.size()) {
        uint16_t key_len= *((uint16_t*) m_data_buffer.ptr(pos));
        uint16_t key_start = pos + 2;
        uint16_t val_len = *((uint16_t*) m_data_buffer.ptr(key_start + key_len));
        uint16_t val_start = key_start + key_len + 2;
        if (key_len && val_len) {
            m_vars.insert(std::make_pair(std::string(m_data_buffer.ptr(key_start), key_len),
                                         std::string(m_data_buffer.ptr(val_start), val_len)));
        }
        pos = val_start + val_len;
    }
}

} // net
} // tasks
