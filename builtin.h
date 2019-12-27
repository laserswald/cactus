#ifndef cact_builtin_h_INCLUDED
#define cact_builtin_h_INCLUDED

typedef struct cact_builtin_table {
    char* name;
    cact_val* (*fn)(cact_val*, cact_env*);
} BuiltinTable;

cact_val* cact_builtin_if(cact_val *x, cact_env *e);
cact_val* cact_builtin_car(cact_val *x, cact_env *unused);
cact_val* cact_builtin_cdr(cact_val *x, cact_env *unused);
cact_val* cact_builtin_cons(cact_val *x, cact_env *unused);
cact_val* cact_builtin_is_nil(cact_val *x, cact_env *unused);
cact_val* cact_builtin_is_pair(cact_val *x, cact_env *unused);
cact_val* cact_builtin_is_number(cact_val *x, cact_env *e);

cact_val* cact_builtin_is_boolean(cact_val *x, cact_env *e);
cact_val* cact_builtin_not(cact_val *x, cact_env *e);

cact_val* cact_builtin_eq(cact_val *x, cact_env *unused);
cact_val* cact_builtin_display(cact_val *x, cact_env *e);
cact_val* cact_builtin_newline(cact_val *x, cact_env *unused);
cact_val* cact_builtin_progn(cact_val *x, cact_env *e);
cact_val* cact_builtin_exit(cact_val *x, cact_env *e);
cact_val* cact_builtin_load(cact_val *x, cact_env *e);

cact_val* cact_builtin_plus(cact_val *x, cact_env *e);
cact_val* cact_builtin_minus(cact_val *x, cact_env *e);
cact_val* cact_builtin_times(cact_val *x, cact_env *e);
cact_val* cact_builtin_divide(cact_val *x, cact_env *e);
cact_val* cact_builtin_load(cact_val *x, cact_env *e);

void cact_make_builtin(cact_env *e, cact_val *x, cact_val *(fn)(cact_val*, cact_env*));
cact_env * cact_make_builtins();

#endif // cact_builtin_h_INCLUDED

