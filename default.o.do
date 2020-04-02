#!/bin/sh

redo-ifchange "$2.c"
cc -o $3 -c "$2.c" -Wall -pedantic -Iinclude -g -DNDEBUG
# cc -MD -MF "$2.d" -o $3 -c "$2.c" -Wall -pedantic -Iinclude -g
# read DEPS <$2.d
#redo-ifchange ${DEPS#*:}
