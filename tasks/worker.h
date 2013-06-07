#ifndef _TASKS_WORKER_H_
#define _TASKS_WORKER_H_

#include <tasks/logging.h>
#include <tasks/ev_wrapper.h>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <sstream>
#include <cassert>

namespace tasks {

	class task;
	class io_task;
	class timer_task;

	// Needed to use std::unique_ptr<>
	class loop_wrapper {
	public:
		struct ev_loop *loop;
	};

	// Signals to enter the leader thread context
	typedef std::function<void(struct ev_loop*)> task_func;

	struct task_func_queue {
		std::queue<task_func> queue;
		std::mutex mutex;
	};

	// Put all io events into a queue instead of handling them directly from handle_io_event as multiple
	// events can fire and we want to promote the next leader after the ev_loop call returns to avoid calling
	// it from multiple threads.
	struct io_event {
		tasks::io_task* task;
		int revents;
	};
		
	class worker {
	public:
		worker(int id);
		virtual ~worker();

		inline int get_id() const {
			return m_id;
		}

		inline std::string get_string() const {
			std::ostringstream os;
			os << "worker(" << m_id << ")";
			return os.str();
		}

		// Executes task_func directly if called in leader thread context or delegates it. Returns true
		// when task_func has been executed.
		inline bool signal_call(task_func f) {
			if (m_leader) {
				// The worker is the leader
				f(m_loop->loop);
				return true;
			} else {
				task_func_queue* tfq = (task_func_queue*) m_signal_watcher.data;
				std::lock_guard<std::mutex> lock(tfq->mutex);
				tfq->queue.push(f);
				ev_async_send(ev_default_loop(0), &m_signal_watcher);
				return false;
			}
		}

		// Called by promote_leader() to hand over the event loop struct to the next worker.
		inline void set_event_loop(std::unique_ptr<loop_wrapper> loop) {
			m_loop = std::move(loop);
			m_leader.store(true);
			ev_set_userdata(m_loop->loop, this);
			m_work_cond.notify_one();
		}

		inline void terminate() {
			m_term.store(true);
			m_work_cond.notify_one();
		}
		
		void handle_io_event(ev_io* watcher, int revents);
		void handle_timer_event(ev_timer* watcher);
		
	private:
		int m_id;
		std::thread m_thread;
		std::unique_ptr<loop_wrapper> m_loop;
		std::atomic<bool> m_leader;
		std::atomic<bool> m_term;
		std::mutex m_work_mutex;
		std::condition_variable m_work_cond;
		std::queue<io_event> m_io_events_queue;
		std::queue<timer_task*> m_timer_events_queue;

		// Every worker has an async watcher to be able to call into the leader thread context.
		ev_async m_signal_watcher;

		void promote_leader();
		void run();
	};
	
	/* CALLBACKS */
	static void ev_io_callback(struct ev_loop* loop, ev_io* w, int e) {
		worker* worker = (tasks::worker*) ev_userdata(loop);
		assert(nullptr != worker);
		worker->handle_io_event(w, e);
	}

	static void ev_async_callback(struct ev_loop* loop, ev_async* w, int events) {
		worker* worker = (tasks::worker*) ev_userdata(loop);
		assert(nullptr != worker);
		task_func_queue* tfq = (tasks::task_func_queue*) w->data;
		assert(nullptr != tfq);
		std::lock_guard<std::mutex> lock(tfq->mutex);
		// Execute all queued functors
		while (!tfq->queue.empty()) {
			assert(worker->signal_call(tfq->queue.front()));
			tfq->queue.pop();
		}
	}

	static void ev_timer_callback(struct ev_loop* loop, ev_timer* w, int revents) {
		worker* worker = (tasks::worker*) ev_userdata(loop);
		assert(nullptr != worker);
		worker->handle_timer_event(w);
	}

} // tasks

#endif // _TASKS_WORKER_H_
