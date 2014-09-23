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

#ifndef _TASKS_EXEC_H_
#define _TASKS_EXEC_H_

#include <tasks/exec_task.h>

namespace tasks {

/*!
 * \brief Execute code in a separate executor thread.
 *
 * Execute code in a separate executor thread that does not block a
 * worker thread. This can be useful to implement async interfaces
 * using libraries that do not provide async interfaces.
 *
 * \param f The functor to execute.
 */
void exec(exec_task::func_t f);

/*!
 * \brief Execute code in a separate executor thread.
 *
 * Execute code in a separate executor thread that does not block a
 * worker thread. This can be useful to implement async interfaces
 * using libraries that do not provide async interfaces.
 *
 * \param f The functor to execute.
 * \param ff The finish functor to be executed when f has been
 *           executed.
 */
void exec(exec_task::func_t f, task::finish_func_void_t ff);

}

#endif // _TASKS_EXEC_H_
