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

#ifndef _TASKS_DISPATCHER_H_
#define _TASKS_DISPATCHER_H_

#include <vector>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <atomic>
#include <unistd.h>

#include <tasks/ev_wrapper.h>
#include <tasks/tools/bitset.h>

namespace tasks {
        
class worker;
class task;
    
struct signal_data;
    
class dispatcher {
public:
    dispatcher(uint8_t num_workers);

    // Use this method to override the number of worker threads. The default is the
    // number of CPU's. This method needs to be called before the first call to
    // instance().
    static void init_workers(uint8_t num_workers) {
        if (nullptr == m_instance) {
            m_instance = std::make_shared<dispatcher>(num_workers);
        }
    }

    static std::shared_ptr<dispatcher> instance() {
        if (nullptr == m_instance) {
            // Create as many workers as we have CPU's per default
            m_instance = std::make_shared<dispatcher>(sysconf(_SC_NPROCESSORS_ONLN));
        }
        return m_instance;
    }

    static void destroy() {
        if (nullptr != m_instance) {
            if (!m_instance->m_term) {
                m_instance->terminate();
                m_instance->join();
            }
            m_instance.reset();
            m_instance = nullptr;
        }
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

    void print_worker_stats() const;
    
private:
    static std::shared_ptr<dispatcher> m_instance;
    std::atomic<bool> m_term;

    // All worker threads
    std::vector<std::shared_ptr<worker> > m_workers;
    uint8_t m_num_workers = 0;

    // State of the workers
    tools::bitset m_workers_busy;

    // Condition variable/mutex used to wait for finishing up 
    std::condition_variable m_finish_cond;
    std::mutex m_finish_mutex;

    ev_signal m_signal;
    
    // Helper to start initial tasks
    bool start_net_io_task(task* task);
    bool start_timer_task(task* task);
};

} // tasks

#endif // _TASKS_DISPATCHER_H_
