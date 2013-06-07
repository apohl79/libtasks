#include <vector>
#include <sys/socket.h>

#include <tasks/dispatcher.h>
#include <tasks/acceptor.h>
#include <tasks/logging.h>
#include <tasks/ev_wrapper.h>

#include <echo_handler.h>

std::atomic<int> stats::m_req_count;
std::atomic<int> stats::m_clients;

bool echo_handler::handle_event(tasks::worker* worker, int events) {
	int fd = get_fd();
	if (events & EV_READ) {
		std::vector<char> buf(1024);
		ssize_t bytes = recvfrom(fd, &buf[0], buf.size(), 0, nullptr, nullptr);
		if (bytes < 0 && errno != EAGAIN) {
			terr("echo_handler: error reading from client file descriptor " << fd << ", errno "
				<< errno << std::endl);
			return false;
		} else if (bytes == 0) {
			std::cout << "echo_handler: client " << fd << " disconnected" << std::endl;
			return false;
		} else if (bytes > 0) {
			tdbg("echo_handler: read " << bytes << " bytes" << std::endl);
			buf.resize(bytes);
			m_write_queue.push(std::move(buf));
		}
	}
	if (events & EV_WRITE) {
		if (!m_write_queue.empty()) {
			std::vector<char>& buf = m_write_queue.front();
			ssize_t bytes = sendto(fd, &buf[m_write_offset], buf.size() - m_write_offset,
								   0, nullptr, 0);
			if (bytes < 0 && errno != EAGAIN) {
				terr("echo_handler: error writing to client file descriptor " << fd << ", errno "
					 << errno << std::endl);
				return false;
			} else if (bytes > 0) {
				tdbg("echo_handler: wrote " << bytes << " bytes" << std::endl);
				if (bytes == (buf.size() - m_write_offset)) {
					// buffer send completely
					m_write_queue.pop();
					stats::inc_req();
				} else {
					m_write_offset += bytes;
				}
			}
		}
	}
	if (m_write_queue.empty()) {
		set_events(EV_READ);
		update_watcher(worker);
	} else {
		set_events(EV_READ|EV_WRITE);
		update_watcher(worker);
	}
	return true;
}

bool stats::handle_event(tasks::worker* worker, int events) {
	std::time_t now = std::time(nullptr);
	std::time_t diff = now - m_last;
	int count;
	count = m_req_count.exchange(0, std::memory_order_relaxed);
	m_last = now;
	int qps = count / diff;
	std::cout << qps << " req/s, num of clients " << m_clients << std::endl;
	return true;
}

int main(int argc, char** argv) {
	stats s;
	tasks::acceptor<echo_handler> srv(12345);
	tasks::dispatcher::get_instance()->run(2, &srv, &s);
	return 0;
}
