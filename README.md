libtasks
========

Overview
--------

libtasks is an I/O task system written in modern C++ that implements the Leader/Followers pattern and uses libev as event loop. It allows for writing event based applications. Network as well as disk I/O operations are supported.

### Leader/Followers Pattern

The Leader/Followers pattern was published by Douglas C. Schmidt, Carlos O’Ryan, Michael Kircher and Irfan Pyarali back in [2000](http://www.kircher-schwanninger.de/michael/publications/lf.pdf).

The idea is to allow one thread at a time – the leader – to wait for an event to occur on a set of I/O handles. Meanwhile, other threads – the followers – can queue up waiting their turn to become the leader. After the current leader thread demultiplexes an event from the I/O handle set, it promotes a follower thread to become the new leader and then dispatches the event to a designated event handler, which processes the event. At this point, the former leader and the new leader thread can execute concurrently.
In detail: multiple former leader threads can process events concurrently while the current leader thread waits on the handle set. After its event processing completes, an idle follower thread waits its turn to become the leader.

The figure below illustrates the states and the valid transitions in the Leader/Followers pattern.

![](https://raw.githubusercontent.com/apohl79/libtasks/master/docs/leader-followers.png "Leader/Followers")

Use cases
---------

libtasks supports the [uwsgi protocol](http://uwsgi-docs.readthedocs.org/en/latest/Protocol.html) as well as [thrift](http://thrift.apache.org/). The uwsgi protocol is supported by web servers like [nginx](http://nginx.org/) or [cherokee](http://cherokee-project.com/) to create high performance web application backends.

### The following things are possible at the moment:

- Implement all kinds of io tasks that use file descriptors, like high performance
  tcp servers 
  
- Implement high performance web application backends using the uwsgi protocol
  and high scale web server front ends like nginx.

- Implement thrift servers using HTTP transport over uwsgi

- Implement HTTP clients

Building
--------

To build libtasks you will need cmake.

### The following options are possible at the moment:

- -DCMAKE_BUILD_TYPE=<type> - if type is "Debug" the library will be build with debug logging enabled. Default is "Release".
- -DDISABLE_TESTS=<option>  - if option is "y" or "Y" the tests will not be build. Default is N.
- -DWITH_EXAMPLES=<option>  - if option is "y" or "Y" the examples will be built too. Default is N.
- -DWITH_PROFILER=<option>  - if option is "y" or "Y" the examples will be build with profiling support. Default is N.

Examples
--------

### An HTTP client

```C++
using namespace tasks::net;

// Create a handler for the HTTP response
class test_handler : public http_response_handler {
public:
    bool handle_response(std::shared_ptr<http_response> response) {
        std::cout << "Got status " << response->status() << std::endl;
        if (response->content_length()) {
            std::cout << "Content:" << std::endl << response->content_p() << std::endl;
        }
        return false;
    }
};

// Send a request
http_sender<test_handler> sender;
auto request = std::make_shared<http_request>("www.google.com", "/");
sender.send(request);
```

### A thrift server with HTTP/uwsgi transport

Consider you want to write a service that looks up information for a given IP address. Look at the [ip_service_server](examples/ip_service_server) example for the details like the thrift IDL.

```C++
using namespace tasks;
using namespace tasks::net;

// We need a handler class that does the actual information lookup.
class ip_service : public uwsgi_thrift_handler<IpServiceIf> {
public:
    void lookup(response_type& result, const int32_t ipv4, const ipv6_type& ipv6) {
        key_value_type kv;
        id_name_type val;
        kv.key.id = 1;
        val.id = 123456;
        kv.values.push_back(val);    
        result.key_values.push_back(kv);
    }
};

// Now we can setup the server
dispatcher::instance()->start();
dispatcher::instance()->add_task(new acceptor<uwsgi_thrift_processor<IpServiceProcessor, ip_service>>(12345));
dispatcher::instance()->join();
```
