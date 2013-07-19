libtasks
========

A simple task system written in C++11 that implements the leader/follower pattern
and uses libev.

Have a look at the examples to get a sense. A macbook pro with a 2.5Ghz I5 can
handle around 40.000 echo requests/s while the generator runs on it as well. The
number of clients is independant from the throughput. See the echoserver example.


What you can do
---------------

The following things are possible at the moment:

- Implement all kinds of io tasks that use file descriptors, like high performance
  tcp servers 
  
- Implement high performance web application backends using the uwsgi protocol
  and high scale web server front ends like nginx.

- Implement thrift servers using HTTP transport over uwsgi

- Implement HTTP clients
