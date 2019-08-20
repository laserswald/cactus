BINARY = cactus
CFLAGS = -g -Wall -Wpedantic -Werror
CC=clang 

SOURCE_FILES := builtin.c env.c eval.c globals.c read.c sexp.c utils.c write.c xmalloc.c 
OBJECT_FILES := $(SOURCE_FILES:.c=.o)

all: $(BINARY)

$(BINARY): $(SOURCE_FILES) slisp.c
	$(CC) $(CFLAGS) -DNDEBUG -o $@ $^

fulltest: $(SOURCE_FILES) tests/fulltest.c
	$(CC) $(CFLAGS) -o $@ $^

test: fulltest slisp-dbg
	./fulltest
	./slisp-dbg t/test.l
	./slisp-dbg t/map.l

tags: $(SOURCE_FILES)
	ctags -R .

clean:
	rm *.o
	rm slisp slisp-dbg
	rm core*

.PHONY: clean test

