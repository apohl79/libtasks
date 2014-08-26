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

#include <tasks/executor.h>
#include <chrono>

namespace tasks {

// An executor dies after 1min idle time per default.
uint32_t executor::m_timeout = 60;

executor::executor()
    : m_busy(true), m_term(false) {
    m_thread.reset(new std::thread(&executor::run, this));
}

void executor::run() {
    tdbg("run: entered" << std::endl);
    while (!m_term) {
        std::unique_lock<std::mutex> lock(m_mutex);
        int idle_runs = 10 * m_timeout; // 10 checks per second
        int run = 0;
        while (!m_cond.wait_for(lock,
                                std::chrono::milliseconds(100),
                                [this] { return nullptr != m_task || m_term; })) {
            if (++run >= idle_runs) {
                // idle timeout
                m_term = true;
            }
        }
        if (!m_term) {
            tdbg("executing task " << m_task << std::endl);
            m_task->execute();
            tdbg("done executing task " << m_task << std::endl);
            if (m_task->auto_delete()) {
                m_task->finish(nullptr);
            }
            m_task = nullptr;
            m_busy = false;
        }
    }
    tdbg("run: leaving" << std::endl);
}

} // tasks
