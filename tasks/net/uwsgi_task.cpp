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

#include <tasks/logging.h>
#include <tasks/net/uwsgi_task.h>

#include <sstream>

namespace tasks {
namespace net {

bool uwsgi_task::handle_event(tasks::worker* /* worker */, int revents) {
    bool success = true;
    try {
        if (EV_READ & revents) {
            m_request.read_data(socket());
            if (m_request.done()) {
                if (UWSGI_VARS == m_request.header().modifier1) {
                    success = handle_request();
                } else {
                    // No suuport for anything else for now
                    std::ostringstream os;
                    os << "uwsgi_task: unsupported uwsgi packet: "
                       << "modifier1=" << (int) m_request.header().modifier1
                       << " datasize=" << m_request.header().datasize
                       << " modifier2=" << (int) m_request.header().modifier2;
                    set_error(os.str());
                    success = false;
                }
            }
        } else if (EV_WRITE & revents) {
            m_response.write_data(socket());
            if (m_response.done()) {
                finish_request();
#if UWSGI_KEEPALIVE == 1
                set_events(EV_READ);
                update_watcher(worker);
#else
                success = false;
#endif
            }
        }
    } catch (tasks::tasks_exception& e) {
        set_error(e.what());
        success = false;
    }
    return success;
}

} // net
} // tasks

