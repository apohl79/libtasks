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

#ifndef _UWSGI_REQUEST_H_
#define _UWSGI_REQUEST_H_

#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <iostream>

#include <tasks/net/uwsgi_structs.h>
#include <tasks/tools/buffer.h>

#ifdef __linux__
#define RECVFROM_FLAGS MSG_DONTWAIT
#else
#define RECVFROM_FLAGS 0
#endif

namespace tasks {
namespace net {
		
class uwsgi_request {
public:
	static std::string NO_VAL;
	
	uwsgi_request() {
		m_header = {0};
	}

	inline const std::string& var(std::string key) const {
		auto it = m_vars.find(key);
		if (m_vars.end() != it) {
			return it->second;
		}
		return NO_VAL;
	}

	inline void print_header() const {
		std::cout << "header:"
				  << " modifier1=" << (int) m_header.modifier1
				  << " datasize=" << m_header.datasize
				  << " modifier2=" << (int) m_header.modifier2
				  << std::endl;
	}

	inline void print_vars() const {
		for (auto kv : m_vars) {
			std::cout << kv.first << " = " << kv.second << std::endl;
		}
	}

	inline void append(const void* data, std::size_t size) {
		m_content_buffer.append(data, size);
	}

	inline std::size_t copy(void* data, std::size_t size) {
		if (m_content_buffer.bytes_left() < size) {
			size = m_content_buffer.bytes_left();
		}
		std::memcpy(data, m_content_buffer.pointer(), size);
		m_content_buffer.move_pointer(size);
		return size;
	}

	bool read_data(int fd);

	inline bool done() const {
		return m_state == DONE;
	}

	inline uwsgi_packet_header& header() {
		return m_header;
	}

	inline void clear() {
		m_state = READY;
		m_header = {0};
		m_data_buffer.clear();
		m_content_buffer.clear();
		if (m_vars.size()) {
			m_vars.clear();
		}
	}

private:
	uwsgi_packet_header m_header;
	tasks::tools::buffer m_data_buffer;
	tasks::tools::buffer m_content_buffer;
	uwsgi_state m_state = READY;
	std::unordered_map<std::string, std::string> m_vars;

	bool read_header(int fd);
	bool read_vars();
};

} // net
} // tasks

#endif // _UWSGI_REQUEST_H_
