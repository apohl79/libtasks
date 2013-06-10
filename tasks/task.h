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

#ifndef _TASKS_TASK_H_
#define _TASKS_TASK_H_

#include <memory>

namespace tasks {

	class worker;
	
	class task {
	public:
		virtual ~task() {}

		// Each task needs to implement the handle_event method. Returns true if the task stays active
		// and false otherwise. The task will be deleted if false is returned and auto_delete()
		// returns true.
		virtual bool handle_event(worker* worker, int events) = 0;

		virtual void stop_watcher(worker* worker) = 0;
		virtual void start_watcher(worker* worker) = 0;

		inline bool auto_delete() const {
			return m_auto_delete;
		}

		inline void disable_auto_delete() {
			m_auto_delete = false;
		}

	private:
		// Default behavior is to delete a task when handle_event returns false. Change this by calling
		// disable_auto_delete().
		bool m_auto_delete = true;
		
	};
	
} // tasks

#endif // _TASKS_TASK_H_
