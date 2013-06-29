#include <iostream>
#include "stats.h"

std::atomic<int> stats::m_req_count;
std::atomic<int> stats::m_clients;

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
