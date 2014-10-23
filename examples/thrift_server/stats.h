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

#ifndef _STATS_H_
#define _STATS_H_

#include <tasks/timer_task.h>
#include <atomic>
#include <ctime>

class stats : public tasks::timer_task {
   public:
    stats() : timer_task(10., 10.) { m_last = std::time(nullptr); }

    bool handle_event(tasks::worker*, int revents);

    static void inc_req() { m_req_count++; }

    static void inc_clients() { m_clients++; }

    static void dec_clients() { m_clients--; }

   private:
    static std::atomic<int> m_req_count;
    static std::atomic<int> m_clients;
    std::time_t m_last;
};

#endif  // _STATS_H_
