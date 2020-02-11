DEPS="core.o builtin.o env.o sexp.o eval.o globals.o number.o read.o write.o utils.o xmalloc.o proc.o sym.o"
redo-ifchange $DEPS
ar -rcs $3 $DEPS

