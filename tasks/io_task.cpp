#include <tasks/worker.h>
#include <tasks/io_task.h>
#include <tasks/logging.h>
#include <unistd.h>

namespace tasks {

	io_task::io_task(int fd, int events) : m_fd(fd), m_events(events) {
		tdbg(get_string() << ": ctor" << std::endl);
		std::unique_ptr<ev_io> io(new ev_io);
		m_io = std::move(io);
		ev_init(m_io.get(), ev_io_callback);
		if (-1 != m_fd) {
			ev_io_set(m_io.get(), m_fd, m_events);
		}
		m_io->data = this;
	}

	io_task::~io_task() {
		tdbg(get_string() << ": dtor" << std::endl);
		// NOTE: The watcher will be stoped by dispose().
		if (-1 != m_fd) {
			close(m_fd);
		}
	}

	void io_task::set_fd(int fd) {
		if (-1 == m_fd) {
			tdbg(get_string() << ": setting file descriptor to " << fd << std::endl);
			m_fd = fd;
			m_change_pending = true;
		} else {
			terr(get_string() << ": set_fd is only allowed once" << std::endl);
		}
	}

	void io_task::set_events(int events) {
		if (m_events != events) {
			tdbg(get_string() << ": setting events to " << events << std::endl);
			m_events = events;
			m_change_pending = true;
		}
	}
	
	void io_task::start_watcher(worker* worker) {
		worker->signal_call([this] (struct ev_loop* loop) {
				if (!ev_is_active(m_io.get())) {
					tdbg(get_string() << ": starting watcher" << std::endl);
					ev_io_start(loop, m_io.get());
				}
			});
	}

	void io_task::stop_watcher(worker* worker) {
		worker->signal_call([this] (struct ev_loop* loop) {
				if (ev_is_active(m_io.get())) {
					tdbg(get_string() << ": stopping watcher" << std::endl);
					ev_io_stop(loop, m_io.get());
				}
			});
	}

	void io_task::update_watcher(worker* worker) {
		if (m_change_pending) {
			worker->signal_call([this] (struct ev_loop* loop) {
					tdbg(get_string() << ": updating watcher" << std::endl);
					bool active = ev_is_active(m_io.get());
					if (active) {
						ev_io_stop(loop, m_io.get());
					}
					ev_io_set(get_watcher(), m_fd, m_events);
					if (active) {
						ev_io_start(loop, m_io.get());
					}
				});
			m_change_pending = false;
		}
	}
		
	void io_task::add_io_task(worker* worker, io_task* task) {		
		worker->signal_call([task] (struct ev_loop* loop) {
				tdbg(task->get_string() << ": adding io_task" << std::endl);
				task->init_watcher();
				ev_io_start(loop, task->get_watcher());
			});
	}

	void io_task::dispose(worker* worker, io_task* task) {
		if (nullptr == task) {
			task = this;
		}
		worker->signal_call([task] (struct ev_loop* loop) {
				tdbg(task->get_string() << ": disposing io_task" << std::endl);
				if (ev_is_active(task->get_watcher())) {
					ev_io_stop(loop, task->get_watcher());
				}
				delete task;
			});
	}

} // tasks
