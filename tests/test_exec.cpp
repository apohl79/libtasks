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

#include <tasks/exec.h>
#include <tasks/executor.h> // for set_timeout
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <set>

#include "test_exec.h"

using namespace tasks;

std::atomic<int> g_state(0);
std::mutex g_mutex;
std::condition_variable g_cond;

#ifndef __clang__
thread_local int t_state = 0;
#else
__thread int t_state = 0;
#endif

void test_exec::run() {
    std::thread::id tid;

    // reduce the idle timeout for executor threads for the tests
    executor::set_timeout(4);

    // create a task
    auto t1 = new exec_task([&tid] {
            tid = std::this_thread::get_id();
            g_state = 1;
            t_state++;
            g_cond.notify_one();
        });
    dispatcher::instance()->add_task(t1);

    CPPUNIT_ASSERT_MESSAGE(std::string("g_state=") + std::to_string(g_state), check_state(1));

    exec([&tid] {
            // we should be in the same thread as previosly
            CPPUNIT_ASSERT(std::this_thread::get_id() == tid);
            g_state = 2;
            t_state++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            g_cond.notify_one();
        });

    exec([&tid] {
            // we should be in the a different thread now
            CPPUNIT_ASSERT(std::this_thread::get_id() != tid);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            g_state = 3;
            t_state++;
            g_cond.notify_one();
        });

    CPPUNIT_ASSERT_MESSAGE(std::string("g_state=") + std::to_string(g_state), check_state(2));
    CPPUNIT_ASSERT_MESSAGE(std::string("g_state=") + std::to_string(g_state), check_state(3));

    // wait until all executors die
    std::this_thread::sleep_for(std::chrono::seconds(5));

    exec([] {
            // we should be in a thread now, that we have not seen before, means t_state must be 0
            CPPUNIT_ASSERT(t_state == 0);
            g_state = 4;
            g_cond.notify_one();
        });

    CPPUNIT_ASSERT_MESSAGE(std::string("g_state=") + std::to_string(g_state), check_state(4));

    // test on_finish
    exec([] {g_state = 5;},
         [] {CPPUNIT_ASSERT(g_state == 5); g_state = 6; g_cond.notify_one();});
    

    CPPUNIT_ASSERT_MESSAGE(std::string("g_state=") + std::to_string(g_state), check_state(6));
}

bool test_exec::check_state(int expected) {
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_cond.wait_for(lock, std::chrono::seconds(5),
                           [expected] { return g_state == expected; });
}
