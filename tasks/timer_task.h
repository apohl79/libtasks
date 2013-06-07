#ifndef _TASKS_TIMER_TASK_H_
#define _TASKS_TIMER_TASK_H_

#include <tasks/task.h>
#include <tasks/ev_wrapper.h>
#include <memory>

namespace tasks {

	class worker;
	
	class timer_task : public task {
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
