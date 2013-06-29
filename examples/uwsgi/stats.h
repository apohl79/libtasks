#ifndef _STATS_H_
#define _STATS_H_

#include <tasks/timer_task.h>
#include <atomic>
#include <ctime>

class stats : public tasks::timer_task {
public:
	stats() : timer_task(10., 10.) {
		m_last = std::time(nullptr);
	}

	bool handle_event(tasks::worker*, int revents);

	static void inc_req() {
		m_req_count++;
	}
		
	static void inc_clients() {
		m_clients++;
	}

	static void dec_clients() {
		m_clients--;
	}

private:
	static std::atomic<int> m_req_count;
	static std::atomic<int> m_clients;
	std::time_t m_last;
};

#endif // _STATS_H_
