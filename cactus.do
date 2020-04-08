#!/bin/sh

. ./config.sh

redo-ifchange main.c libcact.a
$CC $CFLAGS -o $3 main.c libcact.a -DNDEBUG
