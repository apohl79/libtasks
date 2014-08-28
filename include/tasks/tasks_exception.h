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

#ifndef _TASKS_TASKS_EXCEPTION_H_
#define _TASKS_TASKS_EXCEPTION_H_

#include <string>

namespace tasks {

/*!
 * \brief The base exception class.
 */
class tasks_exception : public std::exception {
public:
    tasks_exception(std::string what) : m_what(what) {}
    const char* what() const noexcept {
        return m_what.c_str();
    }
    
private:
    std::string m_what;
};

} // tasks

#endif // _TASKS_TASKS_EXCEPTION_H_
