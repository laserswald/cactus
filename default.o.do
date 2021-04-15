#!/bin/sh

. ./config.sh


if [ $CACT_OPTIMIZE -eq 1 ]; then
    CFLAGS="$CFLAGS -O2 -DNDEBUG"
fi

if [ $CACT_DEBUG -eq 1 ]; then
    CFLAGS="$CFLAGS -g"
fi

if [ $CACT_COVERAGE -eq 1 ]; then
    CFLAGS="$CFLAGS -ftest-coverage -fprofile-arcs"
fi

# echo "$0: CFLAGS is $CFLAGS" >&2

redo-ifchange "$2.c"

$CC $CFLAGS -MD -MF "$2.d" -o "$3" -c "$2.c"

# Read the generated dependency files, and ensure the object file depends
# on them.
read DEPS <$2.d

redo-ifchange ${DEPS#*:}
