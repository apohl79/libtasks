/*
 * Copyright (c) 2013 Andreas Pohl <apohl79 at gmail.com>
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

#ifndef _TASKS_DISPATCHER_H_
#define _TASKS_DISPATCHER_H_

#include <vector>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <atomic>

#include <tasks/ev_wrapper.h>
#include <tasks/tools/bitset.h>

namespace tasks {
        
class worker;
class task;
    
struct signal_data;
    
class dispatcher {
public:
    dispatcher();

    static std::shared_ptr<dispatcher> instance() {
        if (nullptr == m_instance) {
            m_instance = std::make_shared<dispatcher>();
        }
        return m_instance;
    }
        
    // Get a free worker to promote it to the leader.
    std::shared_ptr<worker> free_worker();

    // When a worker finishes his work he returns to the free worker queue.
    void add_free_worker(uint8_t id);

    // Returns the first worker from the workers vector. This can be useful
    // to add tasks in situations where a worker handle is not available.
    inline worker* first_worker() {
        return m_workers[0].get();
    }
        
    // This method starts the system and blocks until terminate() gets called.
    void run(int num, ...);

    // Start the event loop. Do not block.
    void start();
    
    // Wait for the dispatcher to finish
    void join();
    
    // Terminate the workers and die.
    inline void terminate() {
        m_term = true;
        m_finish_cond.notify_one();
    }
    
private:
    static std::shared_ptr<dispatcher> m_instance;

    // All worker threads
    std::vector<std::shared_ptr<worker> > m_workers;
    uint8_t m_num_workers = 0;

    // State of the workers
    tools::bitset m_workers_active;

    // Condition variable/mutex used to wait for finishing up 
    std::condition_variable m_finish_cond;
    std::mutex m_finish_mutex;

    ev_signal m_signal;

    std::atomic<bool> m_term;
    
    // Helper to start initial tasks
    bool start_io_task(task* task);
    bool start_timer_task(task* task);
};

} // tasks

#endif // _TASKS_DISPATCHER_H_
