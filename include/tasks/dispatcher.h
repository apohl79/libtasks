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
#include <list>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <atomic>
#include <unistd.h>

#include <tasks/ev_wrapper.h>
#include <tasks/tools/bitset.h>
#include <tasks/logging.h>

namespace tasks {
        
class worker;
class executor;
class task;
    
struct signal_data;
    
class dispatcher {
    friend class test_exec;

public:
    enum class mode {
        SINGLE_LOOP,
        MULTI_LOOP
    };

    dispatcher(uint8_t num_workers);

    // Use this method to override the number of worker threads. The default is the
    // number of CPU's. This method needs to be called before the first call to
    // instance().
    static void init_workers(uint8_t num_workers) {
        if (nullptr == m_instance) {
            m_instance = std::make_shared<dispatcher>(num_workers);
        }
    }

    // Set the run mode.
    // The default is to run a leader/followers system (MODE_SINGLE_LOOP) in
    // which only one event loop exists that is passed from worker to worker. An
    // alternative is to run an event loop in each worker (MODE_MULTI_LOOP). This can
    // improve the responsiveness and throughput in some situations.
    //
    // Note: This method has to be called before creating the dispatcher singleton.
    //       It will fail when called later.
    //
    // Available Modes:
    //   SINGLE_LOOP (Default)
    //   MULTI_LOOP
    static void init_run_mode(mode m) {
        if (nullptr != m_instance) {
            terr("ERROR: dispatcher::init_run_mode must be called before anything else!"
                 << std::endl);
            assert(false);
        }
        m_run_mode = m;
    }

    static mode run_mode() {
        return m_run_mode;
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

    // Find a free executor. If non is found a new executor gets created.
    std::shared_ptr<executor> free_executor();

    // When a worker finishes his work he returns to the free worker queue.
    void add_free_worker(uint8_t id);

    // Returns the last promoted worker from the workers vector. This can be useful
    // to add tasks in situations where a worker handle is not available.
    inline worker* last_worker() {
        return m_workers[m_last_worker_id].get();
    }

    // Add a task to the system.
    void add_task(task* task);

    // This methods start the system and block until terminate() gets called.
    void run(int num, ...);
    void run(std::vector<tasks::task*>& tasks);

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

    // All executor threads
    std::list<std::shared_ptr<executor> > m_executors;
    std::mutex m_executor_mutex;

    static mode m_run_mode;

    // State of the workers used for maintaining the leader/followers
    tools::bitset m_workers_busy;
    tools::bitset::int_type m_last_worker_id = 0;

    // A round robin counter to add tasks to the system. In case each
    // worker runs it's own event loop this is useful to distribute tasks
    // across the workers.
    std::atomic<uint8_t> m_rr_worker_id;

    // Condition variable/mutex used to wait for finishing up 
    std::condition_variable m_finish_cond;
    std::mutex m_finish_mutex;

    ev_signal m_signal;
};

} // tasks

#endif // _TASKS_DISPATCHER_H_
