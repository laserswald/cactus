TESTS="tests/*_test.c"
redo-ifchange libcact.a $TESTS
cc -o $3 "tests/$2.c" $TESTS libcact.a
