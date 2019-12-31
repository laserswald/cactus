DEPS="builtin.o env.o sexp.o eval.o globals.o number.o read.o write.o utils.o xmalloc.o proc.o load.o bitset.o gc.o"
redo-ifchange $DEPS
ar -rcs $3 $DEPS

