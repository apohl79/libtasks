#!/usr/bin/perl

use strict;
use IO::Socket;

$|++;

my $i = 100;
while (fork() && $i--) {}

my $c = IO::Socket::INET->new(Proto    => "tcp",
			      PeerAddr => "localhost",
			      PeerPort => "12345"
			     ) or die "connection failed";
$c->autoflush(1);

my $data = "echotest";

my $t = time;
my $count = 0;
while (1) {
  $c->send($data);
  my $buf;
  $c->recv($buf, 1024);
}
