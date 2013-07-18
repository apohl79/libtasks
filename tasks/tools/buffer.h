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
#include <streambuf>

namespace tasks {
namespace tools {

class buffer : public std::streambuf {
public:
    buffer() {
        setg(ptr_begin(), ptr_begin(), ptr_end());
        setp(ptr_begin(), ptr_end());
    }
    
	inline char* ptr_write() {
		return pptr();
	}

	inline const char* ptr_write() const {
		return pptr();
	}

    inline char* ptr_read() {
		return gptr();
	}

	inline const char* ptr_read() const {
		return gptr();
	}

	inline char* ptr(std::size_t pos) {
		assert(pos <= m_size);
		return &m_buffer[pos];
	}

	inline const char* ptr(std::size_t pos) const {
		assert(pos <= m_size);
		return &m_buffer[pos];
	}

    inline char* ptr_begin() {
        return &m_buffer[0];
    }

    inline const char* ptr_begin() const {
        return &m_buffer[0];
    }

    inline char* ptr_end() {
        return &m_buffer[m_size];
    }
    
    inline const char* ptr_end() const {
        return &m_buffer[m_size];
    }
    
    inline std::size_t offset_write() const {
        return pptr() - ptr_begin();
    }

    inline std::size_t offset_read() const {
        return gptr() - ptr_begin();
    }

	inline void move_ptr_write(std::size_t s) {
        auto owrite = offset_write();
        setp(ptr(owrite + s), ptr_end());
	}

    inline void move_ptr_read(std::size_t s) {
        auto oread = offset_read();
        setg(ptr_begin(), ptr(oread + s), ptr_end());
	}

	inline void move_ptr_write_abs(std::size_t pos) {
        setp(ptr(pos), ptr_end());
	}

	inline void move_ptr_read_abs(std::size_t pos) {
        setg(ptr_begin(), ptr(pos), ptr_end());
	}

	inline std::size_t to_write() const {
		return ptr_end() - pptr();
	}

	inline std::size_t to_read() const {
		return ptr_end() - gptr();
	}

	inline std::size_t size() const {
		return m_size;
	}

	inline void set_size(std::size_t s) {
        auto oread = offset_read();
        auto owrite = offset_write();
		if (m_buffer.size() < s) {
			m_buffer.resize(s + 1024);
		}
		m_size = s;
        setg(ptr_begin(), ptr(oread), ptr_end());
        setp(ptr(owrite), ptr_end());
 	}

    inline void shrink() {
        m_buffer.resize(m_size);
    }
	
	inline std::size_t buffer_size() {
		return m_buffer.size();
	}
	
	inline std::streamsize write(const char_type* data, std::streamsize size) {
        return xsputn(data, size);
	}

    inline std::streamsize read(char_type* data, std::streamsize size) {
        return xsgetn(data, size);
    }
		
	inline void clear() {
		m_size = 0;
        setg(ptr_begin(), ptr_begin(), ptr_end());
        setp(ptr_begin(), ptr_end());
	}

protected:
    // std::streambuf override
    std::streamsize xsputn(const char_type* s, std::streamsize count) {
        set_size(m_size + count);
        return std::streambuf::xsputn(s, count);
    }
    
    // std::streambuf override
    std::streamsize xsgetn(char_type* s, std::streamsize count) {
        auto size = count;
        auto bytes_left = to_read();
        if (size > bytes_left) {
            size = bytes_left;
        }
        return std::streambuf::xsgetn(s, size);
    }

private:
	std::vector<char> m_buffer;
	std::size_t m_size = 0;
	std::size_t m_offset = 0;

};

} // tools
} // tasks

#endif // _BUFFER_H_
