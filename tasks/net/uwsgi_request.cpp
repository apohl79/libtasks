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

#include <tasks/logging.h>
#include <sys/socket.h>
#include <cstdlib>

#include <tasks/net/uwsgi_request.h>

namespace tasks {
namespace net {

std::string uwsgi_request::NO_VAL;

bool uwsgi_request::read_header(int fd) {
	bool success = false;
	ssize_t bytes = recvfrom(fd, &m_header, sizeof(m_header), RECVFROM_FLAGS, nullptr, nullptr);
	if (bytes < 0 && errno != EAGAIN) {
		terr("uwsgi_request: error reading from client file descriptor " << fd << ", errno "
			 << errno << std::endl);
	} else if (bytes == 0) {
		tdbg("uwsgi_request: client " << fd << " disconnected" << std::endl);
	} else if (bytes == sizeof(m_header)) {
		success = true;
		tdbg("uwsgi_request: read header successfully, " << bytes << " bytes" << std::endl);
	} else {
		terr("uwsgi_task: error reading header" << std::endl);
	}
	return success;
}

bool uwsgi_request::read_data(int fd) {
	bool success = true;
	if (READY == m_state) {
		m_state = READ_HEADER;
		if (success = read_header(fd)) {
			m_state = READ_DATA;
			m_data_buffer.set_size(m_header.datasize);
		}
	}
	if (success && READ_DATA == m_state) {
		ssize_t bytes = recvfrom(fd, m_data_buffer.pointer(), m_data_buffer.bytes_left(),
								 RECVFROM_FLAGS, nullptr, nullptr);
		if (bytes < 0) {
			if (errno != EAGAIN) {
				terr("uwsgi_request: error reading from client file descriptor " << fd << ", errno "
					 << errno << std::endl);
				success = false;
			}
		} else if (bytes == 0) {
			tdbg("uwsgi_request: client " << fd << " disconnected" << std::endl);
			success = false;
		} else if (bytes > 0) {
			m_data_buffer.move_pointer(bytes);
			tdbg("uwsgi_request: read data successfully, " << bytes << " bytes" << std::endl);
		}
		if (success && !m_data_buffer.bytes_left()) {
			if (UWSGI_VARS == m_header.modifier1) {
				success = read_vars();
				if (success) {
					// Check if a http body needs to be read
					std::string content_len_s = var("CONTENT_LENGTH");
					if (NO_VAL != content_len_s) {
						std::size_t content_len_i = std::atoi(content_len_s.c_str());
						m_content_buffer.set_size(content_len_i);
						m_state = READ_CONTENT;
					} else {
						m_state = DONE;
					}
				}
			}
		}
	}
	if (success && READ_CONTENT == m_state) {
		ssize_t bytes = recvfrom(fd, m_content_buffer.pointer(), m_content_buffer.bytes_left(),
								 RECVFROM_FLAGS, nullptr, nullptr);
		if (bytes < 0) {
			if (errno != EAGAIN) {
				terr("uwsgi_request: error reading from client file descriptor " << fd << ", errno "
					 << errno << std::endl);
				success = false;
			}
		} else if (bytes == 0) {
			tdbg("uwsgi_request: client " << fd << " disconnected" << std::endl);
			success = false;
		} else if (bytes > 0) {
			m_content_buffer.move_pointer(bytes);
			tdbg("uwsgi_request: read data successfully, " << bytes << " bytes" << std::endl);
		}
		if (success && !m_content_buffer.bytes_left()) {
			m_state = DONE;
			// Move the pointer the start to enable reading from the buffer. 
			m_content_buffer.move_pointer_abs(0);
		}
	}
	return success;
}

bool uwsgi_request::read_vars() {
	std::size_t pos = 0;
	while (pos < m_data_buffer.size()) {
		uint16_t key_len= *((uint16_t*) m_data_buffer.pointer(pos));
		uint16_t key_start = pos + 2;
		uint16_t val_len = *((uint16_t*) m_data_buffer.pointer(key_start + key_len));
		uint16_t val_start = key_start + key_len + 2;
		if (key_len && val_len) {
			m_vars.insert(std::make_pair(std::string(m_data_buffer.pointer(key_start), key_len),
										 std::string(m_data_buffer.pointer(val_start), val_len)));
		}
		pos = val_start + val_len;
	}
	return true;
}

} // net
} // tasks
