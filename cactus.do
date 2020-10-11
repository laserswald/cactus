#!/bin/sh

. ./config.sh

redo-ifchange main.o libcact.a
$CC $CFLAGS --coverage -o $3 main.o libcact.a -DNDEBUG
