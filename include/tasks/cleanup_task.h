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

#ifndef _TASKS_CLEANUP_TASK_H_
#define _TASKS_CLEANUP_TASK_H_

#include <tasks/timer_task.h>
#include <tasks/disposable.h>
#include <tasks/dispatcher.h>
#include <tasks/logging.h>

namespace tasks {

class cleanup_task : public timer_task {
   public:
    cleanup_task(disposable* d) : timer_task(1., 1.), m_disposable(d) {}

    bool handle_event(worker* worker, int) {
        if (m_disposable->can_dispose()) {
            tdbg("cleanup_task(" << this << "): disposing " << m_disposable << std::endl);
            m_disposable->dispose(worker);
            return false;  // done
        } else {
            return true;  // retry
        }
    }

   private:
    disposable* m_disposable;
};

}  // tasks

#endif  // _TASKS_CLEANUP_TASK_H_
