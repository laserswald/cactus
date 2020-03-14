#!/bin/sh

DEPS="
	lib/bool.o
	lib/core.o
	lib/env.o
	lib/err.o
	lib/eval.o
	lib/number.o
	lib/obj.o
	lib/pair.o
	lib/port.o
	lib/proc.o
	lib/read.o
	lib/rec.o
	lib/store.o
	lib/str.o
	lib/sym.o
	lib/syn.o
	lib/utils.o
	lib/val.o
	lib/vec.o
	lib/write.o
	lib/xmalloc.o
"
redo-ifchange $DEPS
ar -rcs $3 $DEPS

