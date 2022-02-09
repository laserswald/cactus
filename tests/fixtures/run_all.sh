#!/bin/sh

for inputfile in `find tests/fixtures -iname '*.scm'`; do
	printf "testing fixture $inputfile: "

	if ! ./cactus $inputfile
	then
		echo "fail"
		failed=1
	else
        echo "ok"
    fi
done

[ $failed ] && exit 1

