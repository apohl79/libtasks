#!/bin/bash
RUN=$1
CFG_DIR=$2
MODE=$3

nginx -p $CFG_DIR -c nginx.conf

$RUN $MODE
RC=$?

nginx -p $CFG_DIR -c nginx.conf -s stop

exit $RC
