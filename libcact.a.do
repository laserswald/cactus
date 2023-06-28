#!/bin/sh
set -x
DEPS=$(find src -iname '*.c' | grep -v 'mac.c' | grep -v 'main.c' | sed 's/\.c$/.o/g' | tr "\n" ' ')
redo-ifchange $DEPS
ar -rcs $3 $DEPS

