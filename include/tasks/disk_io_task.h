/*
 * Copyright (c) 2013-2014 Andreas Pohl <apohl79 at gmail.com>
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

#include <tasks/event_task.h>
#include <tasks/ev_wrapper.h>
#include <tasks/tools/buffer.h>
#include <tasks/logging.h>
#include <future>

namespace tasks {

class worker;

class disk_io_task : public event_task {
public:
    disk_io_task(int fd, int events, tools::buffer* buf);
    virtual ~disk_io_task();

    inline std::string get_string() const {
        std::ostringstream os;
        os << "disk_io_task(" << this << "," << m_fd << ":" << m_events << ")";
        return os.str();
    }

    inline std::streamsize bytes() const {
        return m_bytes;
    }

    virtual bool handle_event(worker* worker, int events);
    virtual void init_watcher() {}
    virtual void start_watcher(worker* /* worker */) {}
    virtual void stop_watcher(worker* /* worker */) {}

    virtual void dispose(worker* worker = nullptr);

    static std::shared_future<std::streamsize> add_task(disk_io_task* task) {
        assert(nullptr != task);
        return task->op();
    }

private:
    int m_fd = -1;
    int m_events = EV_UNDEF;
    tools::buffer* m_buf = nullptr;
    std::streamsize m_bytes = -1;

    std::shared_future<std::streamsize> op();
};

} // tasks

#endif // _TASKS_IO_TASK_H_
