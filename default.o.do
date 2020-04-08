#!/bin/sh

. ./config.sh

redo-ifchange "$2.c"

if [ $CACT_OPTIMIZE -eq 1 ]; then
    CFLAGS="$CFLAGS -O2 -DNDEBUG"
fi

if [ $CACT_DEBUG -eq 1 ]; then
    CFLAGS="$CFLAGS -g"
fi

$CC $CFLAGS -MD -MF "$2.d" -o $3 -c "$2.c"

# Read the generated dependency files, and ensure the object file depends
# on them.
read DEPS <$2.d
redo-ifchange ${DEPS#*:}
