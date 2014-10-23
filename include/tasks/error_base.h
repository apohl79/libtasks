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

#ifndef _TASKS_ERROR_BASE_H_
#define _TASKS_ERROR_BASE_H_

#include <string>

namespace tasks {

/*!
 * \brief A helper class for basic error reporting.
 */
class error_base {
   public:
    inline bool error() const { return m_error_code > 0; }

    inline uint16_t error_code() const { return m_error_code; }

    inline const std::string& error_message() const { return m_error_message; }

    inline void set_error(uint16_t code, std::string message) {
        m_error_code = code;
        m_error_message = message;
    }

    inline void set_error(std::string message) { set_error(1, message); }

   private:
    uint16_t m_error_code = 0;
    std::string m_error_message;
};

}  // tasks

#endif  // _TASKS_ERROR_BASE_H_
