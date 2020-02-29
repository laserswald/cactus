DEPS="
	xmalloc.o
	utils.o

	globals.o
	core.o
	builtin.o
	store.o
	env.o
	eval.o

	sexp.o
	number.o
	read.o
	write.o
	proc.o
	sym.o
"
redo-ifchange $DEPS
ar -rcs $3 $DEPS

