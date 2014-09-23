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
#include <tasks/exec_task.h>
#include <tasks/dispatcher.h>

namespace tasks {

void exec(exec_task::func_t f) {
    dispatcher::instance()->add_task(new exec_task(f));
}

void exec(exec_task::func_t f, task::finish_func_void_t ff) {
    exec_task* t = new exec_task(f);
    t->on_finish(ff);
    dispatcher::instance()->add_task(t);
}

} // tasks
