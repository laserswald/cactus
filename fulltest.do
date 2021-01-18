#!/bin/sh

. ./config.sh

TESTS=$(find ./tests -iname '*_test.c' | sed 's/\.c$/.o/g')

redo-ifchange libcact.a $TESTS

$CC $CFLAGS -o $3 "tests/$2.c" $TESTS libcact.a
