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

#ifndef _THRIFT_SERVER_WRITER_H_
#define _THRIFT_SERVER_WRITER_H_

#include <string>
#include <boost/shared_ptr.hpp>
#include <protocol/TProtocol.h>

namespace tasks {
namespace tools {

template<class data_type, class transport_type, class protocol_type>
class thrift_server_writer {
public:
	thrift_server_writer(std::string name,
						 boost::shared_ptr<transport_type> t,
						 boost::shared_ptr<protocol_type> p)
		: m_name(name), m_transport(t), m_protocol(p) {}

	inline void write(data_type& d) {
		using namespace apache::thrift::protocol;
		m_protocol->writeMessageBegin(m_name, T_REPLY, 0);
		m_protocol->writeStructBegin("result");
		m_protocol->writeFieldBegin("success", T_STRUCT, 0);
		d.write(m_protocol.get());
		m_protocol->writeFieldEnd();
		m_protocol->writeFieldStop();
		m_protocol->writeStructEnd();
		m_protocol->writeMessageEnd();
		m_transport->writeEnd();
		m_transport->flush();
	}
	
private:
	std::string m_name;
	boost::shared_ptr<transport_type> m_transport;
	boost::shared_ptr<protocol_type> m_protocol;
};

} // tools
} // tasks

#endif // _THRIFT_SERVER_WRITER_H_
