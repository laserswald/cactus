redo-ifchange main.c libcact.a
cc -Iinclude -Wall -pedantic -o $3 main.c libcact.a -DNDEBUG
