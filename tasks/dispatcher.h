#ifndef _TASKS_DISPATCHER_H_
#define _TASKS_DISPATCHER_H_

#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <atomic>
#include <tasks/ev_wrapper.h>

namespace tasks {
		
	class worker;
	class task;
	
	struct signal_data;
	
	class dispatcher {
	public:
		dispatcher();

		static std::shared_ptr<dispatcher> get_instance() {
			if (nullptr == m_instance) {
				m_instance = std::make_shared<dispatcher>();
			}
			return m_instance;
		}
		
		// Get a free worker to promote it to the leader.
		std::shared_ptr<worker> get_free_worker();

		// When a worker finishes his work he returns to the free worker queue.
		void add_free_worker(int id);
		
		// This method starts the system and blocks until finish() gets called. At least one task has to be passed.
		void run(int num, task* task, ...);

		// Terminate the workers and die.
		void finish();
		
	private:
		static std::shared_ptr<dispatcher> m_instance;
		
		// All worker threads
		std::vector<std::shared_ptr<worker> > m_workers;
		int m_num_workers = 0;

		// Free workers queue up here (TODO: implement lock free queue)
		std::queue<int> m_worker_queue;
		std::mutex m_worker_mutex;

		// Condition variable/mutex used to wait for finishing up 
		std::condition_variable m_finish_cond;
		std::mutex m_finish_mutex;

		ev_signal m_signal;

		// Helper to start initial tasks
		bool start_io_task(task* task);
		bool start_timer_task(task* task);
	};
	
} // tasks

#endif // _TASKS_DISPATCHER_H_
