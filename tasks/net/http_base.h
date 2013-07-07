#ifndef _HTTP_BASE_H_
#define _HTTP_BASE_H_

#include <unordered_map>
#include <iostream>
#include <cstring>
#include <cstdlib>

#include <tasks/net/io_state.h>
#include <tasks/tools/buffer.h>

#define CRLF "\r\n"
#define CRLF_SIZE 2

#ifdef __linux__
#define SENDTO_FLAGS MSG_NOSIGNAL
#else
#define SENDTO_FLAGS 0
#endif

namespace tasks {
namespace net {

class http_base {
public:
    static const std::string NO_VAL;

    virtual ~http_base() {}

    inline void set_state(io_state state) {
        m_state = state;
    }

    inline io_state state() const {
        return m_state;
    }

    inline void set_header(std::string header, std::string value) {
        m_headers[header] = value;
        if (header == "Content-Length") {
            m_content_length = std::atoi(value.c_str());
        }
    }

    inline const std::string& header(std::string name) const {
        auto h = m_headers.find(name);
        if (m_headers.end() != h) {
            return h->second;
        }
        return NO_VAL;
    }

    inline std::size_t content_length() const {
        return m_content_length;
    }

    inline void append(std::string s) {
        m_content_buffer.append(s.c_str(), s.length());
    }

    inline void append(const void* data, std::size_t size) {
        m_content_buffer.append(data, size);
    }

    inline std::size_t copy(void *data, std::size_t size) {
		if (m_content_buffer.bytes_left() < size) {
			size = m_content_buffer.bytes_left();
		}
		std::memcpy(data, m_content_buffer.pointer(), size);
		m_content_buffer.move_pointer(size);
		return size;
    }

    virtual void prepare_data_buffer() = 0;
    
    bool write_data(int fd);

    bool read_data(int fd);

    inline void print() const {
        std::cout << std::string(m_data_buffer.pointer(0), m_data_buffer.size());
        if (m_content_buffer.size()) {
            std::cout << std::string(m_content_buffer.pointer(0), m_content_buffer.size());
        }
    }

    inline bool done() const {
        return m_state == DONE;
    }

    virtual void clear() {
        m_data_buffer.clear();
        m_content_buffer.clear();
        m_content_length = 0;
        if (m_headers.size() > 0) {
            m_headers.clear();
        }
        m_state = READY;
    }
    
protected:
    tasks::tools::buffer m_data_buffer;
    tasks::tools::buffer m_content_buffer;
    io_state m_state = READY;
    std::unordered_map<std::string, std::string> m_headers;
    std::size_t m_content_length = 0;


    bool write_headers(int fd);
    bool write_content(int fd);
};

} // net
} // tasks

#endif // _HTTP_BASE_H_
