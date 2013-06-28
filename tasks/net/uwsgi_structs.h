/*
 * Copyright (c) 2013 Andreas Pohl <apohl79 at gmail.com>
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

#ifndef _UWSGI_STRUCTS_H_
#define _UWSGI_STRUCTS_H_

namespace tasks {
	namespace net {

		enum uwsgi_state : uint8_t {
			READY,
				DONE,
				READ_HEADER,
				READ_DATA,
				READ_CONTENT,
				WRITE_DATA,
				WRITE_CONTENT
				};

		enum uwsgi_packet_type : uint8_t {
			UWSGI_VARS = 0
				};

		struct __attribute__((__packed__)) uwsgi_packet_header {
			uint8_t modifier1;
			uint16_t datasize;
			uint8_t modifier2;
		};

	} // net
} // tasks

#endif // _UWSGI_STRUCTS_H_
