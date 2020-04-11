#!/bin/sh

for inputfile in `find tests/fixtures -iname '*.scm'`; do
	printf "testing fixture $inputfile: "

	expectedfile="${inputfile%.scm}.exp"
	outputfile="${inputfile%.scm}.out"
	./cactus $inputfile > $outputfile

	if ! cmp $outputfile $expectedfile; then
		echo "fail"
		failed=$(true)
	else
        echo "ok"
    fi
done

[ $failed -eq $(true)] && exit 1

