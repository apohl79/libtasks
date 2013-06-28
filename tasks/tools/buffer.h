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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include <cstring>
#include <cassert>

namespace tasks {
	namespace tools {

		class buffer {
		public:
			inline char* pointer() {
				return &m_buffer[m_offset];
			}

			inline const char* pointer() const {
				return &m_buffer[m_offset];
			}

			inline char* pointer(std::size_t pos) {
				assert(pos <= m_size);
				return &m_buffer[pos];
			}

			inline const char* pointer(std::size_t pos) const {
				assert(pos <= m_size);
				return &m_buffer[pos];
			}

			inline void move_pointer(std::size_t s) {
				m_offset += s;
				assert(m_offset <= m_size);
			}

			inline void move_pointer_abs(std::size_t pos) {
				m_offset = pos;
				assert(m_offset <= m_size);
			}

			inline std::size_t bytes_left() const {
				return m_size - m_offset;
			}

			inline std::size_t size() const {
				return m_size;
			}

			inline void set_size(std::size_t s) {
				if (m_buffer.size() < s) {
					m_buffer.resize(s);
				}
				m_size = s;
			}
	
			inline std::size_t buffer_size() {
				return m_buffer.size();
			}

			inline void resize(std::size_t s) {
				m_buffer.resize(s);
			}
	
			inline void append(const void* data, std::size_t size) {
				if (m_buffer.size() < m_size + size) {
					m_buffer.resize(m_size + size + 1024);
				}
				std::memcpy(&m_buffer[m_size], data, size);
				m_size += size;
			}
		
			inline void clear() {
				m_size = 0;
				m_offset = 0;
			}
	
		private:
			std::vector<char> m_buffer;
			std::size_t m_size = 0;
			std::size_t m_offset = 0;

		};

	} // tools
} // tasks

#endif // _BUFFER_H_
