#!/bin/sh

. ./config.sh

redo-ifchange lib/*.c lib/*.h libcact.a
$CC $CFLAGS -o $3 src/main.c lib/*.c libcact.a -DNDEBUG
