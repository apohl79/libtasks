#ifndef _TASKS_TASK_H_
#define _TASKS_TASK_H_

#include <memory>

namespace tasks {

	class worker;
	
	class task {
	public:
		virtual ~task() {}

		// Each task needs to implement the handle_event method. Returns true on success and false
		// otherwise. The task will be deleted if false is returned.
		virtual bool handle_event(worker* worker, int events) = 0;

		inline bool delete_after_error() const {
			return m_delete_after_error;
		}

		inline void disable_delete_after_error() {
			m_delete_after_error = false;
		}

	private:
		// Default behavior is to delete a task when handle_event returns false. Change this by calling
		// disable_delete_after_error().
		bool m_delete_after_error = true;
		
	};
	
} // tasks

#endif // _TASKS_TASK_H_
