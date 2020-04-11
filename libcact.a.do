#!/bin/sh

DEPS=$(find lib -iname '*.c' | sed 's/\.c$/.o/g')



redo-ifchange $DEPS

ar -rcs $3 $DEPS

