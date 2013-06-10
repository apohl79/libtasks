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
#include <tasks/timer_task.h>
#include <tasks/logging.h>
#include <unistd.h>

namespace tasks {

	timer_task::timer_task(double after, double repeat) : m_after(after), m_repeat(repeat) {
		tdbg(get_string() << ": ctor" << std::endl);
		std::unique_ptr<ev_timer> timer(new ev_timer);
		m_timer = std::move(timer);
		ev_timer_init(m_timer.get(), tasks_event_callback<ev_timer*>, after, repeat);
		m_timer->data = this;
	}

	timer_task::~timer_task() {
		tdbg(get_string() << ": dtor" << std::endl);
	}
	
	void timer_task::start_watcher(worker* worker) {
		worker->signal_call([this] (struct ev_loop* loop) {
				if (!ev_is_active(m_timer.get())) {
					tdbg(get_string() << ": starting watcher" << std::endl);
					ev_timer_start(loop, m_timer.get());
				}
			});
	}

	void timer_task::stop_watcher(worker* worker) {
		worker->signal_call([this] (struct ev_loop* loop) {
				if (ev_is_active(m_timer.get())) {
					tdbg(get_string() << ": stopping watcher" << std::endl);
					ev_timer_stop(loop, m_timer.get());
				}
			});
	}

} // tasks
