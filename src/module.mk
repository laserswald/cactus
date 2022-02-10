
BINARY_SRCS += \
	src/main.c

LIBRARY_SRCS += \
	src/bool.c \
	src/builtin.c \
	src/core.c \
	src/env.c \
	src/err.c \
	src/main.c \
	src/number.c \
	src/obj.c \
	src/pair.c \
	src/port.c \
	src/proc.c \
	src/read.c \
	src/rec.c \
	src/store.c \
	src/str.c \
	src/sym.c \
	src/syn.c \
	src/utils.c \
	src/val.c \
	src/vec.c \
	src/write.c \
	src/xmalloc.c
	
include src/compiler/module.mk
include src/evaluator/module.mk
