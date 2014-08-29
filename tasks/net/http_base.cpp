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

#include <sys/socket.h>
#include <tasks/logging.h>

#include <tasks/net/http_base.h>
#include <tasks/net/socket.h>

namespace tasks {
namespace net {

const std::string http_base::NO_VAL;

void http_base::write_data(socket& sock) {
    // Fill the data buffer in ready state
    if (io_state::READY == m_state) {
        prepare_data_buffer();
        m_state = io_state::WRITE_DATA;
    }
    // Write data buffer
    if (io_state::WRITE_DATA == m_state) {
        write_headers(sock);
        if (!m_data_buffer.to_read()) {
            if (m_content_buffer.size()) {
                m_state = io_state::WRITE_CONTENT;
            } else {
                m_state = io_state::DONE;
            }
        }
    }
    // Write content buffer
    if (io_state::WRITE_CONTENT == m_state) {
        write_content(sock);
        if (!m_content_buffer.to_read()) {
            m_state = io_state::DONE;
        }
    }
}

void http_base::write_headers(socket& sock) {
    std::streamsize bytes = sock.write(m_data_buffer.ptr_read(), m_data_buffer.to_read());
    if (bytes > 0) {
        tdbg("http_base: wrote data successfully, " << bytes << "/" << m_data_buffer.size()
             << " bytes" << std::endl);
        m_data_buffer.move_ptr_read(bytes);
    }
}

void http_base::write_content(socket& sock) {
    std::streamsize bytes = sock.write(m_content_buffer.ptr_read(), m_content_buffer.to_read());
    if (bytes > 0) {
        tdbg("http_base: wrote content successfully, " << bytes << "/" << m_content_buffer.size()
             << " bytes" << std::endl);
        m_content_buffer.move_ptr_read(bytes);
    }
}

} // net
} // tasks
