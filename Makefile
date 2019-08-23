BINARY = cactus
CFLAGS = -g -Wall -Wpedantic -Werror
CC=gcc

SOURCE_FILES := builtin.c env.c eval.c globals.c read.c sexp.c utils.c write.c xmalloc.c 
OBJECT_FILES := $(SOURCE_FILES:.c=.o)

all: $(BINARY) fulltest

$(BINARY): $(SOURCE_FILES) main.c
	$(CC) $(CFLAGS) -DNDEBUG -o $@ $^

fulltest: $(SOURCE_FILES) tests/fulltest.c
	$(CC) $(CFLAGS) -o $@ $^

test: fulltest
	./fulltest

tags: $(SOURCE_FILES)
	ctags -R .

clean:
	rm $(BINARY) core* fulltest

.PHONY: clean test

