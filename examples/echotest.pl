#!/usr/bin/perl

use strict;
use threads;
use IO::Socket;

$|++;

my $host = $ARGV[0];
$host ||= "localhost";
my $i = 700;

my @threads;
while ($i--) {
    push @threads, threads->create(\&run);
}
foreach my $t (@threads) {
    $t->join();
}

sub run {
    my $c = IO::Socket::INET->new(Proto    => "tcp",
				  PeerAddr => $host,
				  PeerPort => "12345"
				 ) or die "connection failed";
    $c->autoflush(1);

    my $data = "echotest";
    my $count = 0;
    while (1) {
	$c->send($data);
	my $buf;
	$c->recv($buf, 1024);
    }
}
