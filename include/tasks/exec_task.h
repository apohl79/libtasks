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

#ifndef _TASKS_EXEC_TASK_H_
#define _TASKS_EXEC_TASK_H_

#include <tasks/task.h>
#include <tasks/logging.h>
#include <functional>

namespace tasks {

class exec_task : public task {
public:
    typedef std::function<void ()> func_t;
    
    exec_task(func_t f) : m_func(f) {}
    virtual ~exec_task() {}

    virtual void execute() {
        tdbg("executing m_func" << std::endl);
        m_func();
    }

private:
    func_t m_func;
};

} // tasks

#endif // _TASKS_EXEC_TASK_H_
