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

#include <tasks/worker.h>
#include <tasks/net_io_task.h>
#include <tasks/logging.h>

namespace tasks {

net_io_task::net_io_task(int events) : m_events(events) {
    tdbg(get_string() << ": ctor" << std::endl);
    std::unique_ptr<ev_io> io(new ev_io);
    m_io = std::move(io);
    ev_init(m_io.get(), tasks_event_callback<ev_io*>);
    m_io->data = this;
}

net_io_task::net_io_task(net::socket& socket, int events) : net_io_task(events) {
    m_socket = socket;
    if (-1 != m_socket.fd()) {
        ev_io_set(m_io.get(), m_socket.fd(), m_events);
    }
}

net_io_task::~net_io_task() {
    tdbg(get_string() << ": dtor" << std::endl);
    // NOTE: The watcher will be stoped by dispose().
    m_socket.close();
}

void net_io_task::set_socket(net::socket& socket) {
    if (-1 == m_socket.fd()) {
        tdbg(get_string() << ": setting file descriptor to " << socket.fd() << std::endl);
        m_socket = socket;
        m_change_pending = true;
    } else {
        terr(get_string() << ": set_socket is only allowed once" << std::endl);
    }
}

void net_io_task::set_events(int events) {
    if (m_events != events) {
        tdbg(get_string() << ": setting events to " << events << std::endl);
        m_events = events;
        m_change_pending = true;
    }
}

void net_io_task::start_watcher(worker* worker) {
    sync_worker(worker);
    worker->signal_call([this] (struct ev_loop* loop) {
        if (!ev_is_active(m_io.get())) {
            tdbg(get_string() << ": starting watcher" << std::endl);
            ev_io_start(loop, m_io.get());
        }
    });
}

void net_io_task::stop_watcher(worker* worker) {
    sync_worker(worker);
    worker->signal_call([this] (struct ev_loop* loop) {
            if (ev_is_active(m_io.get())) {
                tdbg(get_string() << ": stopping watcher" << std::endl);
                ev_io_stop(loop, m_io.get());
            }
        });
}

void net_io_task::update_watcher(worker* worker) {
    sync_worker(worker);
    if (m_change_pending) {
        worker->signal_call([this] (struct ev_loop* loop) {
                tdbg(get_string() << ": updating watcher" << std::endl);
                bool active = ev_is_active(m_io.get());
                if (active) {
                    ev_io_stop(loop, m_io.get());
                }
                ev_io_set(m_io.get(), m_socket.fd(), m_events);
                if (active) {
                    ev_io_start(loop, m_io.get());
                }
            });
        m_change_pending = false;
    }
}

void net_io_task::add_task(worker* worker, net_io_task* task) {
    worker->signal_call([task] (struct ev_loop* loop) {
            tdbg(task->get_string() << ": adding net_io_task" << std::endl);
            task->init_watcher();
            ev_io_start(loop, task->watcher());
        });
}

void net_io_task::add_task(net_io_task* task) {
    dispatcher::instance()->add_task(task);
}

void net_io_task::dispose(worker* worker) {
    if (nullptr == worker) {
        if (nullptr != m_worker) {
            // multi loop mode: use the worker the task is assigned to
            worker = m_worker;
        } else {
            // single loop mode: use the last executing worker
            worker = dispatcher::instance()->last_worker();
        }
    }
    worker->signal_call([this] (struct ev_loop* loop) {
            if (ev_is_active(watcher())) {
                tdbg(get_string() << ": disposing net_io_task" << std::endl);
                ev_io_stop(loop, watcher());
            }
            delete this;
        });
}

} // tasks
