#!/usr/bin/perl
#
# Copyright (c) 2013 Andreas Pohl <apohl79 at gmail.com>
#
# This file is part of libtasks.
#
# libtasks is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libtasks is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libtasks.  If not, see <http://www.gnu.org/licenses/>.

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
