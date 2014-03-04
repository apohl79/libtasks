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

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>

namespace tasks {
namespace tools {

class spinlock {
public:
    spinlock() : m_lock(false) {}
    
    inline void lock() {
        while (m_lock.exchange(true)) {}
    }
    
    inline void unlock() {
        m_lock = false;
    }

private:
    std::atomic<bool> m_lock;
};

} // tools
} // tasks

#endif // _SPINLOCK_H_
