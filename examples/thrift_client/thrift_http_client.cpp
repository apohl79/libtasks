#include <iostream>
#include <arpa/inet.h>
#include <protocol/TBinaryProtocol.h>
#include <transport/THttpClient.h>
#include <boost/shared_ptr.hpp>
#include "test_service.h"

int main(int argc, char** argv) {
	using namespace apache::thrift::protocol;
	using namespace apache::thrift::transport;
	boost::shared_ptr<THttpClient> transport(new THttpClient("localhost", 8080, "/test?id=42"));
	boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
	test_serviceClient client(protocol);

	try {
		transport->open();

		std::cout << "Sending request" << std::endl;
		id_name m;
		client.lookup(m);		
		std::cout << "lookup(id=" << m.id << ", name=" << m.name << ")" << std::endl;

		transport->close();
	} catch (TTransportException& e) {
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
