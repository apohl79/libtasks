#ifndef _IP_SERVICE_H_
#define _IP_SERVICE_H_

#include <tasks/net/uwsgi_thrift_handler.h>

class ip_service : public tasks::net::uwsgi_thrift_handler<IpServiceIf> {
public:
    void lookup(response_type& result, const int32_t ipv4, const ipv6_type& ipv6);
};

#endif // _IP_SERVICE_H_
