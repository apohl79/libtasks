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

#ifndef _UWSGI_THRIFT_TRANSPORT_H_
#define _UWSGI_THRIFT_TRANSPORT_H_

#include <transport/TVirtualTransport.h>

namespace tasks {
namespace net {

template<class T>
class uwsgi_thrift_transport :
	public apache::thrift::transport::TVirtualTransport<uwsgi_thrift_transport<T> > {
public:
	uwsgi_thrift_transport(T* r) : m_uwsgi_obj(r) {}
	~uwsgi_thrift_transport() {}

	uint32_t read(uint8_t* data, int32_t size) {
		return m_uwsgi_obj->copy(data, size);
	}

	void write(const uint8_t* data, uint32_t size) {
		m_uwsgi_obj->append(data, size);
	}
	
	void flush() {}

private:
	T* m_uwsgi_obj;
};

} // net
} // tasks

#endif // _UWSGI_THRIFT_TRANSPORT_H_
