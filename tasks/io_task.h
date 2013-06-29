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

#ifndef _TASKS_IO_TASK_H_
#define _TASKS_IO_TASK_H_

#include <tasks/task.h>
#include <tasks/ev_wrapper.h>
#include <memory>
#include <sstream>

namespace tasks {

	class worker;
	
	class io_task : public task {
	public:
		io_task(int fd, int events);
		virtual ~io_task();

		inline std::string get_string() const {
			std::ostringstream os;
			os << "io_task(" << m_fd << ":" << m_events << ")";
			return os.str();
		}

		inline int fd() const {
			return m_fd;
		}

		inline int events() const {
			return m_events;
		}

		inline ev_io* watcher() const {
			return m_io.get();
		}

		inline void init_watcher() {
			ev_io_set(m_io.get(), m_fd, m_events);
		}
		
		void start_watcher(worker* worker);
		void stop_watcher(worker* worker);
		void update_watcher(worker* worker);

		virtual void dispose(worker* worker);

	protected:
		void set_fd(int fd);
		void set_events(int events);
		void add_io_task(worker* worker, io_task* task);
		
	private:
		std::unique_ptr<ev_io> m_io;
		int m_fd = -1;
		int m_events = EV_UNDEF;
		bool m_change_pending = false;
	};
	
} // tasks

#endif // _TASKS_IO_TASK_H_
