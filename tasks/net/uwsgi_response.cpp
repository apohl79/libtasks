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

#include <iostream>
#include <sys/socket.h>
#include <tasks/logging.h>

#include <tasks/net/uwsgi_response.h>
#include <tasks/tools/itostr.h>

namespace tasks {
	namespace net {

		bool uwsgi_response::write_data(int fd) {
			bool success = true;
			// Fill the data buffer in ready state
			if (READY == m_state) {
				// Status line
				m_data_buffer.append("HTTP/1.1 ", 9);
				m_data_buffer.append(m_status.c_str(), m_status.length());
				m_data_buffer.append(CRLF, CRLF_SIZE);
				// Headers
				for (auto kv : m_headers) {
					m_data_buffer.append(kv.first.c_str(), kv.first.length());
					m_data_buffer.append(": ", 2);
					m_data_buffer.append(kv.second.c_str(), kv.second.length());
					m_data_buffer.append(CRLF, CRLF_SIZE);
				}
				std::string ct = "Content-Length: "
					+ tasks::tools::itostr<std::size_t>(m_content_buffer.size());
				m_data_buffer.append(ct.c_str(), ct.length());
				m_data_buffer.append(CRLF, CRLF_SIZE);
				m_data_buffer.append(CRLF, CRLF_SIZE);
				m_state = WRITE_DATA;
			}
			// Write data buffer
			if (WRITE_DATA == m_state) {
				ssize_t bytes = sendto(fd, m_data_buffer.pointer(), m_data_buffer.bytes_left(),
									   MSG_NOSIGNAL, nullptr, 0);
				if (bytes < 0 && errno != EAGAIN) {
					terr("uwsgi_response: error writing to client file descriptor " << fd << ", errno "
						 << errno << std::endl);
					success = false;
				} else if (bytes > 0) {
					tdbg("uwsgi_response: wrote data successfully, " << bytes << "/" << m_data_buffer.size()
						 << " bytes" << std::endl);
					m_data_buffer.move_pointer(bytes);
					if (!m_data_buffer.bytes_left()) {
						if (m_content_buffer.size()) {
							m_state = WRITE_CONTENT;
						} else {
							m_state = DONE;
						}
					}	
				} else {
					tdbg("uwsgi_response: no data bytes written" << std::endl);
				}
			}
			// Write content buffer
			if (WRITE_CONTENT == m_state) {
				ssize_t bytes = sendto(fd, m_content_buffer.pointer(), m_content_buffer.bytes_left(),
									   MSG_NOSIGNAL, nullptr, 0);
				if (bytes < 0 && errno != EAGAIN) {
					terr("uwsgi_response: error writing to client file descriptor " << fd << ", errno "
						 << errno << std::endl);
					success = false;
				} else if (bytes > 0) {
					tdbg("uwsgi_response: wrote content successfully, " << bytes << "/" << m_content_buffer.size()
						 << " bytes" << std::endl);
					m_content_buffer.move_pointer(bytes);
					if (!m_content_buffer.bytes_left()) {
						m_state = DONE;
					}	
				} else {
					tdbg("uwsgi_response: no data bytes written" << std::endl);
				}
			}
			return success;
		}

	} // net
} // tasks
