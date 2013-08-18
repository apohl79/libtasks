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

#include <tasks/dispatcher.h>
#include <tasks/worker.h>
#include <tasks/task.h>
#include <tasks/io_task.h>
#include <tasks/timer_task.h>
#include <tasks/logging.h>
#include <cassert>
#include <cstdarg>
#include <chrono>
#include <unistd.h>

namespace tasks {

std::shared_ptr<dispatcher> dispatcher::m_instance = nullptr;
    
static void handle_signal(struct ev_loop* loop, ev_signal* sig, int revents) {
    dispatcher* d = (dispatcher*) sig->data;
    assert(nullptr != d);
    d->terminate();
}

dispatcher::dispatcher()
    : m_term(false),
      m_num_workers(sysconf(_SC_NPROCESSORS_ONLN) * 2),
      m_workers_active(tools::bitset(m_num_workers)) {    
    // Initialize the event loop structure
    ev_default_loop(0);
    // Create workers 
    tdbg("dispatcher: number of cpus is " << m_num_workers << std::endl);
    // The first thread becomes the leader
    for (uint8_t i = 0; i < m_num_workers; i++) {
        std::shared_ptr<worker> w = std::make_shared<worker>(i);
        assert(nullptr != w);
        m_workers.push_back(w);
    }
    // Setup signal handlers
    ev_signal_init(&m_signal, handle_signal, SIGINT);
    m_signal.data = this;
    ev_signal_start(ev_default_loop(0), &m_signal);
}

bool dispatcher::start_io_task(task* task) {
    bool success = false;
    io_task* io_task = dynamic_cast<tasks::io_task*>(task);
    if (nullptr != io_task) {
        io_task->init_watcher();
        ev_io_start(ev_default_loop(0), io_task->watcher());
        success = true;
    }
    return success;
}

bool dispatcher::start_timer_task(task* task) {
    bool success = false;
    timer_task* timer_task = dynamic_cast<tasks::timer_task*>(task);
    if (nullptr != timer_task) {
        ev_timer_start(ev_default_loop(0), timer_task->watcher());
        success = true;
    }
    return success;
}

void dispatcher::run(int num, ...) {
    // Start tasks if passed
    if (num > 0) {
        va_list tasks;
        va_start(tasks, num);
        for (int i = 0; i < num; i++) {
            tasks::task* t = va_arg(tasks, tasks::task*);
            // Try to find a proper task specialization and start a watcher
            bool started = start_io_task(t);
            if (!started) started = start_timer_task(t);
        }
        va_end(tasks);
    }

    // Start the event loop
    start();
    
    // Now we park this thread until someone calls finish()
    join();
}

void dispatcher::start() {
    // Create an event loop and pass it to a worker
    std::unique_ptr<loop_wrapper> loop(new loop_wrapper);
    loop->loop = ev_default_loop(0);
    m_workers_active.set(0);
    m_workers[0]->set_event_loop(std::move(loop));
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
        if (m_workers_active.first(id)) {
            tdbg("dispatcher: free_worker(" << id << ")" << std::endl);
            m_workers_active.set(id);
            return m_workers[id];
        }
    }
    return nullptr;
}

void dispatcher::add_free_worker(uint8_t id) {
    tdbg("dispatcher: add_free_worker(" << (unsigned int) id << ")" << std::endl);
    m_workers_active.unset(id);
}

} // tasks
