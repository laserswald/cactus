#!/bin/sh

exec >&2

remove_artifacts () {
	for xtn in o d a gcov gcda gcno; do
	    echo "rm -f '$1/*.$xtn'"
	    rm -f $1/*.$xtn
    done
}

rm -f cactus fulltest
remove_artifacts .
remove_artifacts lib
remove_artifacts tests
