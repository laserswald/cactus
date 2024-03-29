CFLAGS = -g -Isrc -Ilib -Wall -pedantic
LIBRARY = libcact.a
BINARY = cactus
TESTBIN = cactus-test

HELPER_LIBRARY_SOURCES = 
LIBRARY_SRCS =
BINARY_SRCS =
TESTBIN_SRCS =

include lib/module.mk
include src/module.mk
include tests/module.mk

LIBRARY_OBJS = $(LIBRARY_SRCS:.c=.o)
BINARY_OBJS = $(BINARY_SRCS:.c=.o)
TESTBIN_OBJS = $(TESTBIN_SRCS:.c=.o)

all: $(BINARY) test

test: $(TESTBIN) $(BINARY)
	exec ./$(TESTBIN)
	tests/fixtures/run_all.sh

clean:
	rm -f $(LIBRARY_SRCS:.c=.d) $(LIBRARY_OBJS) $(LIBRARY)
	rm -f $(BINARY_SRCS:.c=.d) $(BINARY_OBJS) $(BINARY)
	rm -f $(TESTBIN_SRCS:.c=.d) $(TESTBIN_OBJS) $(TESTBIN)

format: $(LIBRARY_SRCS) $(BINARY_SRCS) $(TESTBIN_SRCS)
	astyle --style=1tbs $^

.c.o:
	$(CC) $(CFLAGS) -MD -MF $(<:.c=.d) -c -o $@ $<

$(BINARY): $(BINARY_OBJS) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

$(TESTBIN): $(TESTBIN_OBJS) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBRARY): $(LIBRARY_OBJS)
	$(AR) rcs $@ $^

-include $(LIBRARY_SRCS:.c=.d)
-include $(BINARY_SRCS:.c=.d)

.PHONY: all test clean format
