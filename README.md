libtasks
========

Overview
--------

libtasks is a simple task system written in modern C++ that implements the leader/follower pattern and uses libev. It allows for writing event based applications and supports network and disk IO.

libtasks supports the [uwsgi protocol](http://uwsgi-docs.readthedocs.org/en/latest/Protocol.html). This protocol is supported by web servers like [nginx](http://nginx.org/) or [cherokee](http://cherokee-project.com/) and can be used to create high performance web applications.


What you can do
---------------

The following things are possible at the moment:

- Implement all kinds of io tasks that use file descriptors, like high performance
  tcp servers 
  
- Implement high performance web application backends using the uwsgi protocol
  and high scale web server front ends like nginx.

- Implement thrift servers using HTTP transport over uwsgi

- Implement HTTP clients
