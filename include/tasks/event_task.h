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

#ifndef _TASKS_EVENT_TASK_H_
#define _TASKS_EVENT_TASK_H_

#include <tasks/task.h>

namespace tasks {

class event_task : public task {
public:
    virtual ~event_task() {}

    // Each task needs to implement the handle_event method. Returns true if the task stays active
    // and false otherwise. The task will be deleted if false is returned and auto_delete()
    // returns true.
    virtual bool handle_event(worker* worker, int events) = 0;

    virtual void init_watcher() = 0;
    virtual void stop_watcher(worker* worker) = 0;
    virtual void start_watcher(worker* worker) = 0;

};

} // tasks

#endif // _TASKS_EVENT_TASK_H_
