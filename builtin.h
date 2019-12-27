#ifndef builtin_h_INCLUDED
#define builtin_h_INCLUDED

typedef struct builtin_table {
    char* name;
    cact_val* (*fn)(cact_val*, cact_env*);
} BuiltinTable;

cact_val* builtin_if(cact_val *x, cact_env *e);
cact_val* builtin_car(cact_val *x, cact_env *unused);
cact_val* builtin_cdr(cact_val *x, cact_env *unused);
cact_val* builtin_cons(cact_val *x, cact_env *unused);
cact_val* builtin_is_nil(cact_val *x, cact_env *unused);
cact_val* builtin_is_pair(cact_val *x, cact_env *unused);
cact_val* builtin_is_number(cact_val *x, cact_env *e);

cact_val* builtin_is_boolean(cact_val *x, cact_env *e);
cact_val* builtin_not(cact_val *x, cact_env *e);

cact_val* builtin_eq(cact_val *x, cact_env *unused);
cact_val* builtin_display(cact_val *x, cact_env *e);
cact_val* builtin_newline(cact_val *x, cact_env *unused);
cact_val* builtin_progn(cact_val *x, cact_env *e);
cact_val* builtin_exit(cact_val *x, cact_env *e);
cact_val* builtin_load(cact_val *x, cact_env *e);

cact_val* builtin_plus(cact_val *x, cact_env *e);
cact_val* builtin_minus(cact_val *x, cact_env *e);
cact_val* builtin_times(cact_val *x, cact_env *e);
cact_val* builtin_divide(cact_val *x, cact_env *e);
cact_val* builtin_load(cact_val *x, cact_env *e);

void make_builtin(cact_env *e, cact_val *x, cact_val *(fn)(cact_val*, cact_env*));
cact_env * make_builtins();

#endif // builtin_h_INCLUDED

