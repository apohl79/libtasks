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

#include <tasks/worker.h>
#include <chrono>

namespace tasks {

	worker::worker(int id) : m_id(id), m_thread(&worker::run, this) {
		m_term.store(false);
		// Initialize and add the threads async watcher
		ev_async_init(&m_signal_watcher, tasks_async_callback);
		m_signal_watcher.data = new task_func_queue;
		ev_async_start(ev_default_loop(0), &m_signal_watcher);
	}

	worker::~worker() {
		tdbg(get_string() << ": dtor" << std::endl);
		m_term.store(true);
		m_thread.join();
		task_func_queue* tfq = (task_func_queue*) m_signal_watcher.data;
		delete tfq;
	}
	
	void worker::run() {
		// Wait for a short while before entering the loop to allow
		// the dispatcher to finish its initialization.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (!m_term) {
			// Wait until promoted to the leader thread
			if (!m_leader) {
				tdbg(get_string() << ": waiting..." << std::endl);
				std::unique_lock<std::mutex> lock(m_work_mutex);
				// Use wait_for to check the term flag
				while (m_work_cond.wait_for(lock, std::chrono::milliseconds(100)) == std::cv_status::timeout
					   && !m_leader
					   && !m_term) {}
			}

			// Became leader, so execute the event loop
			while (m_leader && !m_term) {
				tdbg(get_string() << ": running event loop" << std::endl);
				ev_loop(m_loop->loop, EVLOOP_ONESHOT);
				// Check if events got fired
				if (!m_events_queue.empty()) {
					// Now promote the next leader and call the event
					// handlers
					promote_leader();
					// Handle events
					while (!m_events_queue.empty()) {
						event event = m_events_queue.front();
						if (event.task->handle_event(this, event.revents)) {
							// We activate the watcher again as true
							// was returned.
							event.task->start_watcher(this);
						} else {
							if (event.task->auto_delete()) {
								event.task->finish(this);
							}
						}
						m_events_queue.pop();
					}
				}
			}

			if (!m_term) {
				// Add this worker as available worker
				dispatcher::instance()->add_free_worker(id());
			} else {
				// Shutdown, the leader terminates the loop
				if (m_leader) {
					// FIXME: Iterate over all watchers and delete
					// registered tasks.
					ev_unloop (m_loop->loop, EVUNLOOP_ALL);
				}
			}
		}
	}

} // tasks
