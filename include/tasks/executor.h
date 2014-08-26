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

#ifndef _TASKS_EXECUTOR_H_
#define _TASKS_EXECUTOR_H_

#include <tasks/exec_task.h>
#include <tasks/logging.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace tasks {

class executor {
    friend class test_exec;

public:
    executor();

    virtual ~executor() {
        terminate();
        m_thread->join();
        tdbg("terminated" << std::endl);
    }

    inline bool busy() const {
        return m_busy;
    }

    inline void set_busy() {
        m_busy = true;
    }
    
    inline void add_task(exec_task* t) {
        tdbg("add_task " << t << std::endl);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_task = t;
        }
        m_cond.notify_one();
    }

    inline void terminate() {
        tdbg("terminating" << std::endl);
        m_term = true;
        m_cond.notify_one();
    }

    inline bool terminated() const {
        return m_term;
    }

    static void set_timeout(uint32_t timeout) {
        m_timeout = timeout;
    }

private:
    std::atomic<bool> m_busy;
    std::atomic<bool> m_term;
    exec_task* m_task = nullptr;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    static uint32_t m_timeout;
    std::unique_ptr<std::thread> m_thread;

    void run();
};

} // tasks

#endif // _TASKS_EXECUTOR_H_
