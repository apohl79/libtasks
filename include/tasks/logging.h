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

#ifndef _TASKS_LOGGING_H_
#define _TASKS_LOGGING_H_

#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#ifdef LOGMUTEX
#include <mutex>
namespace tasks {
extern std::mutex g_log_mutex;
}
#define _LOGMUTEX std::lock_guard<std::mutex> _log_lock(tasks::g_log_mutex)
#else
#define _LOGMUTEX
#endif

// Using the C time variants as std::put_time is not implemented yet.
#define tlog(s,m)                                                       \
    {                                                                   \
        _LOGMUTEX;                                                      \
        s << "["                                                        \
          << std::setw(14)                                              \
          << std::this_thread::get_id() << " "                          \
          << std::setw(16)                                              \
          << __FILE__ << ":"                                            \
          << std::setw(3)                                               \
          << std::setfill('0')                                          \
          << __LINE__ << "] "                                           \
          << std::setfill(' ')                                          \
          << m << std::flush;                                           \
    }

#ifdef DEBUG
#define tdbg(m) tlog(std::cout,m)
#else
#define tdbg(m)
#endif

#define terr(m) tlog(std::cerr,m)

#endif // _TASKS_LOGGING_H_
