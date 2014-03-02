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

bool http_base::write_data(int fd) {
    bool success = true;
    // Fill the data buffer in ready state
    if (READY == m_state) {
        prepare_data_buffer();
        m_state = WRITE_DATA;
    }
    // Write data buffer
    if (WRITE_DATA == m_state) {
        success = write_headers(fd);
        if (success && !m_data_buffer.to_read()) {
            if (m_content_buffer.size()) {
                m_state = WRITE_CONTENT;
            } else {
                m_state = DONE;
            }
        }
    }
    // Write content buffer
    if (success && WRITE_CONTENT == m_state) {
        success = write_content(fd);
        if (success && !m_content_buffer.to_read()) {
            m_state = DONE;
        }
    }
    return success;
}

bool http_base::write_headers(int fd) {
    bool success = true;
    ssize_t bytes = sendto(fd, m_data_buffer.ptr_read(), m_data_buffer.to_read(),
                           SENDTO_FLAGS, nullptr, 0);
    if (bytes < 0 && errno != EAGAIN) {
        terr("http_base: error writing to client file descriptor " << fd << ", errno "
             << errno << std::endl);
        success = false;
    } else if (bytes > 0) {
        tdbg("http_base: wrote data successfully, " << bytes << "/" << m_data_buffer.size()
             << " bytes" << std::endl);
        m_data_buffer.move_ptr_read(bytes);
    } else {
        tdbg("http_base: no data bytes written" << std::endl);
    }
    return success;
}

bool http_base::write_content(int fd) {
    bool success = true;
    ssize_t bytes = sendto(fd, m_content_buffer.ptr_read(), m_content_buffer.to_read(),
                           SENDTO_FLAGS, nullptr, 0);
    if (bytes < 0 && errno != EAGAIN) {
        terr("http_base: error writing to client file descriptor " << fd << ", errno "
             << errno << std::endl);
        success = false;
    } else if (bytes > 0) {
        tdbg("http_base: wrote content successfully, " << bytes << "/" << m_content_buffer.size()
             << " bytes" << std::endl);
        m_content_buffer.move_ptr_read(bytes);
    } else {
        tdbg("http_base: no data bytes written" << std::endl);
    }
    return success;
}

} // net
} // tasks
