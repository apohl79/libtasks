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

#include <tasks/dispatcher.h>
#include <tasks/worker.h>
#include <tasks/task.h>
#include <tasks/net_io_task.h>
#include <tasks/timer_task.h>
#include <tasks/logging.h>
#include <cassert>
#include <cstdarg>
#include <chrono>

namespace tasks {

std::shared_ptr<dispatcher> dispatcher::m_instance = nullptr;
dispatcher::mode dispatcher::m_run_mode = mode::SINGLE_LOOP;
    
static void handle_signal(struct ev_loop* /* loop */ , ev_signal* sig, int /* revents */) {
    dispatcher* d = (dispatcher*) sig->data;
    assert(nullptr != d);
    d->terminate();
}

dispatcher::dispatcher(uint8_t num_workers)
    : m_term(false),
      m_num_workers(num_workers),
      m_workers_busy(tools::bitset(m_num_workers)),
      m_rr_worker_id(0) {
    // Initialize the event loop structure
    struct ev_loop* loop_raw = ev_default_loop(0);
    // Create workers 
    tdbg("dispatcher: number of cpus is " << (int) m_num_workers << std::endl);
    // The first thread becomes the leader or each thread gets its own loop
    for (uint8_t i = 0; i < m_num_workers; i++) {
        std::unique_ptr<loop_t> loop = nullptr;
        if (mode::MULTI_LOOP == m_run_mode) {
            if (nullptr == loop_raw) {
                loop_raw = ev_loop_new(0);
            }
            assert(loop_raw != nullptr);
            loop.reset(new loop_t(loop_raw));
            // Force the next iteration to create a new loop struct.
            loop_raw = nullptr;
        }
        std::shared_ptr<worker> w = std::make_shared<worker>(i, loop);
        assert(nullptr != w);
        m_workers.push_back(w);
        m_workers_busy.set(i);
    }
    // Setup signal handlers
    ev_signal_init(&m_signal, handle_signal, SIGINT);
    m_signal.data = this;
    ev_signal_start(ev_default_loop(0), &m_signal);
}

void dispatcher::run(int num, ...) {
    // Start tasks if passed
    if (num > 0) {
        va_list tasks;
        va_start(tasks, num);
        for (int i = 0; i < num; i++) {
            tasks::task* t = va_arg(tasks, tasks::task*);
            add_task(t);
        }
        va_end(tasks);
    }

    // Start the event loop
    start();
    
    // Now we park this thread until someone calls finish()
    join();
}

void dispatcher::run(std::vector<tasks::task*>& tasks) {
    for (auto t : tasks) {
        add_task(t);
    }
    
    // Start the event loop
    start();
    
    // Now we park this thread until someone calls finish()
    join();
}

void dispatcher::start() {
    if (mode::SINGLE_LOOP == m_run_mode) {
        // Create an event loop and pass it to a worker
        std::unique_ptr<loop_t> loop(new loop_t);
        loop->ptr = ev_default_loop(0);
        m_workers_busy.unset(0);
        m_workers[0]->set_event_loop(loop);
    }
}

void dispatcher::join() {
    std::unique_lock<std::mutex> lock(m_finish_mutex);
    while (!m_term &&
           m_finish_cond.wait_for(lock, std::chrono::milliseconds(1000)) == std::cv_status::timeout) {}
    tdbg("dispatcher: terminating workers" << std::endl);
    for (auto w : m_workers) {
        w->terminate();
    }
    tdbg("dispatcher: finished" << std::endl);
}

std::shared_ptr<worker> dispatcher::free_worker() {
    if (m_num_workers > 1) {
        tools::bitset::int_type id;
        if (m_workers_busy.next(id, m_last_worker_id)) {
            m_last_worker_id = id;
            tdbg("dispatcher: free_worker(" << id << ")" << std::endl);
            m_workers_busy.unset(id);
            return m_workers[id];
        }
    }
    return nullptr;
}

void dispatcher::add_free_worker(uint8_t id) {
    tdbg("dispatcher: add_free_worker(" << (unsigned int) id << ")" << std::endl);
    m_workers_busy.set(id);
}

void dispatcher::print_worker_stats() const {
    for (auto &w: m_workers) {
        terr(w->get_string() << ": number of handled events is " << w->events_count() << std::endl);
    }
}

void dispatcher::add_task(task* task) {
    worker* worker = nullptr;
    switch (m_run_mode) {
    case mode::MULTI_LOOP:
        // In multi loop mode we pick a worker by round robin
        worker = m_workers[m_rr_worker_id++ % m_num_workers].get();
        break;
    default:
        // In single loop mode use the current worker
        worker = worker::get();
        // If we get called from some other thread than a worker we pick the last active worker
        if (nullptr == worker) {
            worker = m_workers[m_last_worker_id].get();
        }
    }
    task->init_watcher();
    task->start_watcher(worker);
}

} // tasks
