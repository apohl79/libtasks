#!/bin/bash
RUN=$1
CFG_DIR=$2

nginx -p $CFG_DIR -c nginx.conf

$RUN
RC=$?

nginx -p $CFG_DIR -c nginx.conf -s stop

exit $RC
