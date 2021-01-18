#!/bin/sh

for inputfile in `find tests/fixtures -iname '*.scm'`; do
	printf "testing fixture $inputfile: "

	./cactus $inputfile

	if test $? -ne "0"; then
		echo "fail"
		failed=$(true)
	else
        echo "ok"
    fi
done

[ $failed -eq $(true) ] && exit 1

