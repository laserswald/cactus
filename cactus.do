#!/bin/sh

. ./config.sh

redo-ifchange src/*.c src/*.h libcact.a
$CC $CFLAGS -o $3 src/main.c src/linenoise.c libcact.a -DNDEBUG
