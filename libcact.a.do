#!/bin/sh

DEPS=$(find lib -iname '*.c' | sed 's/\.c$/.o/g' | tr "\n" ' ')
redo-ifchange $DEPS
ar -rcs $3 $DEPS

