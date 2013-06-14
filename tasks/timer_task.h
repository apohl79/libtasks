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

#ifndef _TASKS_TIMER_TASK_H_
#define _TASKS_TIMER_TASK_H_

#include <tasks/task.h>
#include <tasks/ev_wrapper.h>
#include <memory>

namespace tasks {

	class worker;
	
	class timer_task : public virtual task {
	public:
		timer_task(double after, double repeat);
		virtual ~timer_task();

		inline std::string get_string() const {
			return "timer_task";
		}
		
		inline ev_timer* get_watcher() const {
			return m_timer.get();
		}

		inline double get_after() const {
			return m_after;
		} 

		inline double get_repeat() const {
			return m_repeat;
		} 

		void start_watcher(worker* worker);
		void stop_watcher(worker* worker);
		
	private:
		std::unique_ptr<ev_timer> m_timer;
		double m_after = 0;
		double m_repeat = 0.;
	};
	
} // tasks

#endif // _TASKS_TIMER_TASK_H_
