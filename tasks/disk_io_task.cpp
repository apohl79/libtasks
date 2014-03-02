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
#include <tasks/disk_io_task.h>
#include <tasks/logging.h>
#include <unistd.h>

namespace tasks {

disk_io_task::disk_io_task(int fd, int events, tools::buffer* buf)
    : m_fd(fd), m_events(events), m_buf(buf) {
    tdbg(get_string() << ": ctor" << std::endl);
}

disk_io_task::disk_io_task(int fd, int events, tools::buffer* buf, std::streamsize* ret)
    : m_fd(fd), m_events(events), m_buf(buf), m_ret(ret) {
    tdbg(get_string() << ": ctor" << std::endl);
}

disk_io_task::~disk_io_task() {
    tdbg(get_string() << ": dtor" << std::endl);
}

void disk_io_task::op() {
    // run the io op in a separate thread
    m_handle = std::async(std::launch::async, [this] {
            switch (m_events) {
            case EV_READ:
                tdbg(get_string() << ": calling read()" << std::endl);
                m_bytes = read(m_fd, m_buf->ptr_write(), m_buf->to_write());
                tdbg(get_string() << ": read() returned " << m_bytes << std::endl);
                if (m_bytes > 0) {
                    m_buf->move_ptr_write(m_bytes);
                }
                break;
            case EV_WRITE:
                tdbg(get_string() << ": calling write()" << std::endl);
                m_bytes = write(m_fd, m_buf->ptr_read(), m_buf->to_read());
                tdbg(get_string() << ": write() returned " << m_bytes << std::endl);
                break;
            default:
                m_bytes = -1;
                terr(get_string() << ": events has to be either EV_READ or EV_WRITE" << std::endl);
            }
            if (nullptr != m_ret) {
                *m_ret = m_bytes;
            }
            // fire an event
            event e = {this, m_events};
            worker::add_async_event(e);
        });
}

bool disk_io_task::handle_event(worker* worker, int events) {
    tdbg(get_string() << ": handle_event" << std::endl);
    return false;
}

void disk_io_task::dispose(worker* worker) {
    worker->signal_call([this] (struct ev_loop* loop) {
            tdbg(get_string() << ": disposing disk_io_task" << std::endl);
            delete this;
        });
}

} // tasks
