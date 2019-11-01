#ifndef builtin_h_INCLUDED
#define builtin_h_INCLUDED

typedef struct builtin_table {
	char* name;
	Sexp* (*fn)(Sexp*, Env*);
} BuiltinTable;

Sexp* builtin_if(Sexp *x, Env *e);
Sexp* builtin_car(Sexp *x, Env *unused);
Sexp* builtin_cdr(Sexp *x, Env *unused);
Sexp* builtin_cons(Sexp *x, Env *unused);
Sexp* builtin_is_nil(Sexp *x, Env *unused);
Sexp* builtin_is_pair(Sexp *x, Env *unused);
Sexp* builtin_is_number(Sexp *x, Env *e);
Sexp* builtin_eq(Sexp *x, Env *unused);
Sexp* builtin_display(Sexp *x, Env *e);
Sexp* builtin_newline(Sexp *x, Env *unused);
Sexp* builtin_progn(Sexp *x, Env *e);
Sexp* builtin_exit(Sexp *x, Env *e);
Sexp* builtin_load(Sexp *x, Env *e);

Sexp* builtin_plus(Sexp *x, Env *e);
Sexp* builtin_minus(Sexp *x, Env *e);
Sexp* builtin_times(Sexp *x, Env *e);
Sexp* builtin_divide(Sexp *x, Env *e);
Sexp* builtin_load(Sexp *x, Env *e);

void make_builtin(Env *e, Sexp *x, Sexp *(fn)(Sexp*, Env*));
Env * make_builtins();

#endif // builtin_h_INCLUDED

