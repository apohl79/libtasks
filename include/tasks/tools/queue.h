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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <tasks/tools/spinlock.h>
#include <mutex>

namespace tasks {
namespace tools {

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

/*
 * A thread safe queue
 *
 * Thx Herb Sutter for this implementation!
 */
template<typename T>
class queue {
public:
    queue() : m_first(new node(T())), m_last(m_first) {}
    ~queue() {
        while (nullptr != m_first) {
            node* n = m_first;
            m_first = m_first->next;
            delete n;
        }
    }

    bool pop(T& res) {
        // No scope lock here to release the lock before deleting the
        // old node
        m_pop_lock.lock();
        if (nullptr != m_first->next) {
            node* old = m_first;
            m_first = m_first->next;
            // for big data types we should optimize here and move the real copy
            // out of the critical section
            res = m_first->val;
            m_pop_lock.unlock();
            delete old;
            return true;
        }
        m_pop_lock.unlock();
        return false;
    }

    bool push(const T& v) {
        node* n = new node(v);
        std::lock_guard<spinlock> lock(m_push_lock);
        m_last->next = n;
        m_last = n;
        return true;
    }
    
private:
    struct node {
        node(T v) : val(v), next(nullptr) {}
        T val;
        std::atomic<node*> next;
        char pad[CACHE_LINE_SIZE - sizeof(T) - sizeof(std::atomic<node*>)];
    };
    
    char pad0[CACHE_LINE_SIZE];
    node* m_first;
    char pad1[CACHE_LINE_SIZE];
    spinlock m_pop_lock;
    char pad2[CACHE_LINE_SIZE];
    node* m_last;
    char pad3[CACHE_LINE_SIZE];
    spinlock m_push_lock;
    char pad4[CACHE_LINE_SIZE];
};

} // tools
} // tasks

#endif // _QUEUE_H_
