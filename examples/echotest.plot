#!/usr/local/bin/gnuplot

infile=system("echo $IN");
outfile=system("echo $OUT");

set terminal postscript eps color enhanced size 10,6;
set output outfile; 
set xlabel "clients";
set grid;

set multiplot layout 2, 1;

set ylabel "requests per second"; 
plot infile using 1:2 title "req/s" with lines smooth bezier;

set ylabel "avg time per req in millisecs";
plot infile using 1:3 title "ms/req" with lines smooth bezier;

unset multiplot;
