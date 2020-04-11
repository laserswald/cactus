#!/bin/sh

. ./config.sh

redo-ifchange main.c libcact.a
$CC $CFLAGS --coverage -o $3 main.c libcact.a -DNDEBUG
