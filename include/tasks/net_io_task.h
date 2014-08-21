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

#ifndef _TASKS_NET_IO_TASK_H_
#define _TASKS_NET_IO_TASK_H_

#include <tasks/worker.h>
#include <tasks/event_task.h>
#include <tasks/net/socket.h>
#include <tasks/ev_wrapper.h>
#include <memory>
#include <sstream>

namespace tasks {

class worker;

class net_io_task : public event_task {
public:
    net_io_task(int events);
    net_io_task(net::socket& socket, int events);
    virtual ~net_io_task();

    inline std::string get_string() const {
        std::ostringstream os;
        os << "net_io_task(" << this << "," << m_socket.fd() << ":" << m_events << ")";
        return os.str();
    }

    inline net::socket& socket() {
        return m_socket;
    }

    inline int events() const {
        return m_events;
    }

    inline ev_io* watcher() const {
        return m_io.get();
    }

    void init_watcher() {
        ev_io_set(m_io.get(), m_socket.fd(), m_events);
    }

    // Start a watcher in the context of the given worker
    void start_watcher(worker* worker);
    // Stop a watcher in the context of the given worker
    void stop_watcher(worker* worker);
    // Udate a watcher in the context of the given worker
    void update_watcher(worker* worker);

    virtual void dispose(worker* worker = nullptr);

    // This public method can be used to add io tasks outside of a worker thread
    // context. If io tasks should be created within the context of a worker thread,
    // you should use the protected non static method and pass a worker thread
    // pointer.
    static void add_task(net_io_task* task);

protected:
    void set_socket(net::socket& socket);
    void set_events(int events);
    void add_task(worker* worker, net_io_task* task);

private:
    std::unique_ptr<ev_io> m_io;
    net::socket m_socket;
    int m_events = EV_UNDEF;
    bool m_change_pending = false;

    // For multi loop mode a task does not leave the context of a worker thread, as
    // each thread runs its own event loop. That also means this worker has to execute
    // a dispose action. As dispose allows to be called from outside of the task
    // system (a non worker thread context), a handle to the worker the task belongs
    // to is needed.
    worker* m_worker = nullptr;
};

} // tasks

#endif // _TASKS_NET_IO_TASK_H_
