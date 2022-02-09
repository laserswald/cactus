CC = tcc
CFLAGS = -g -bt 20 -Iinclude -Wall -pedantic

LIBRARY = libcact.a
BINARY = cactus
TESTBIN = cactus-test

LIBRARY_SRCS = $(shell find lib -iname '*.c')
LIBRARY_OBJS = $(patsubst %.c,%.o, $(LIBRARY_SRCS))

BINARY_SRCS = $(shell find src -iname '*.c')
BINARY_OBJS = $(patsubst %.c,%.o, $(BINARY_SRCS))

TESTBIN_SRCS = $(shell find tests -iname '*.c')
TESTBIN_OBJS = $(patsubst %.c,%.o, $(TESTBIN_SRCS)) 

all: $(BINARY) test

test: $(TESTBIN) $(BINARY)
	exec ./$(TESTBIN) -v
	tests/fixtures/run_all.sh

clean:
	rm -f $(LIBRARY_SRCS:.c=.d) $(LIBRARY_OBJS) $(LIBRARY)
	rm -f $(BINARY_SRCS:.c=.d) $(BINARY_OBJS) $(BINARY)
	rm -f $(TESTBIN_SRCS:.c=.d) $(TESTBIN_OBJS) $(TESTBIN)

format: $(LIBRARY_SRCS) $(BINARY_SRCS) $(TESTBIN_SRCS)
	astyle --style=1tbs $^
	

%.o: %.c 
	$(CC) $(CFLAGS) -MD -MF $(<:.c=.d) -c -o $@ $<

$(BINARY): $(BINARY_OBJS) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^
	
$(TESTBIN): $(TESTBIN_OBJS) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBRARY): $(LIBRARY_OBJS)	
	$(AR) rcs $@ $^

-include $(LIBRARY_SRCS:.c=.d)

.PHONY: all test clean format
