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

#ifndef _TASKS_TASK_H_
#define _TASKS_TASK_H_

#include <functional>
#include <vector>

namespace tasks {

class worker;

class task {
public:
    typedef std::function<void (worker* worker)> finish_func_worker_t;
    typedef std::function<void ()> finish_func_void_t;
    struct finish_func_t {
        finish_func_t(finish_func_worker_t f) : m_type(0), m_f_worker(f) {}
        finish_func_t(finish_func_void_t f) : m_type(1), m_f_void(f) {}

        void operator()(worker* worker) {
            switch (m_type) {
            case 0:
                m_f_worker(worker);
                break;
            case 1:
                m_f_void();
                break;
            }
        }

        int m_type = 0;
        finish_func_worker_t m_f_worker;
        finish_func_void_t m_f_void;
    };

    virtual ~task() {}

    inline bool auto_delete() const {
        return m_auto_delete;
    }

    inline void disable_auto_delete() {
        m_auto_delete = false;
    }

    void finish(worker* worker = nullptr);

    // If a task finishes it can execute callback functions. Note that no locks will be used at this
    // level.
    inline void on_finish(finish_func_worker_t f) {
        m_finish_funcs.push_back(finish_func_t(f));
    }

    inline void on_finish(finish_func_void_t f) {
        m_finish_funcs.push_back(finish_func_t(f));
    }

private:
    // Default behavior is to delete a task when handle_event returns false. Change this by calling
    // disable_auto_delete().
    bool m_auto_delete = true;

    std::vector<finish_func_t> m_finish_funcs;
};

} // tasks

#endif // _TASKS_TASK_H_
