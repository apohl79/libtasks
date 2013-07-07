#include <sys/socket.h>
#include <tasks/logging.h>

#include <tasks/net/http_base.h>

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
        if (success && !m_data_buffer.bytes_left()) {
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
        if (success && !m_content_buffer.bytes_left()) {
            m_state = DONE;
        }
    }
    return success;
}

bool http_base::write_headers(int fd) {
    bool success = true;
    ssize_t bytes = sendto(fd, m_data_buffer.pointer(), m_data_buffer.bytes_left(),
                           SENDTO_FLAGS, nullptr, 0);
    if (bytes < 0 && errno != EAGAIN) {
        terr("http_base: error writing to client file descriptor " << fd << ", errno "
             << errno << std::endl);
        success = false;
    } else if (bytes > 0) {
        tdbg("http_base: wrote data successfully, " << bytes << "/" << m_data_buffer.size()
             << " bytes" << std::endl);
        m_data_buffer.move_pointer(bytes);
    } else {
        tdbg("http_base: no data bytes written" << std::endl);
    }
    return success;
}

bool http_base::write_content(int fd) {
    bool success = true;
    ssize_t bytes = sendto(fd, m_content_buffer.pointer(), m_content_buffer.bytes_left(),
                           SENDTO_FLAGS, nullptr, 0);
    if (bytes < 0 && errno != EAGAIN) {
        terr("http_base: error writing to client file descriptor " << fd << ", errno "
             << errno << std::endl);
        success = false;
    } else if (bytes > 0) {
        tdbg("http_base: wrote content successfully, " << bytes << "/" << m_content_buffer.size()
             << " bytes" << std::endl);
        m_content_buffer.move_pointer(bytes);
    } else {
        tdbg("http_base: no data bytes written" << std::endl);
    }
    return success;
}

} // net
} // tasks
