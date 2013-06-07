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

		inline int get_fd() const {
			return m_fd;
		}

		inline int get_events() const {
			return m_events;
		}

		inline ev_io* get_watcher() const {
			return m_io.get();
		}

		inline void init_watcher() {
			ev_io_set(m_io.get(), m_fd, m_events);
		}
		
		void start_watcher(worker* worker);
		void stop_watcher(worker* worker);
		void update_watcher(worker* worker);

	protected:
		void set_fd(int fd);
		void set_events(int events);
		void add_io_task(worker* worker, io_task* task);
		// Call this to destroy a task. The watcher will be stopped and
		// delete will be called.
		void dispose(worker* worker, io_task* task = nullptr);
		
	private:
		std::unique_ptr<ev_io> m_io;
		int m_fd = -1;
		int m_events = EV_UNDEF;
		bool m_change_pending = false;
	};
	
} // tasks

#endif // _TASKS_IO_TASK_H_
