#ifndef cact_builtin_h_INCLUDED
#define cact_builtin_h_INCLUDED

#include "proc.h"
#include "err.h"

/**
 * Check all the arguments in the argument list and
 */
int
cact_unpack_args(cact_context_t *cact, cact_value_t arglist, const char *format, ...);

/*
 * A structure for quickly adding new native functions.
 */

typedef struct cact_builtin {
	char* name;
	cact_native_func_t fn;
} cact_builtin_t;

/* Define a native procedure in the global default namespace */
void 
cact_define_builtin(cact_context_t *, const char *, cact_native_func_t);

/* Define a set of procedures in the global default namespace */
void 
cact_define_builtin_array(cact_context_t *cact, cact_builtin_t *builtins, size_t len);

#define DEFINE_TYPE_PREDICATE_BUILTIN(name, fn) \
cact_value_t \
name(cact_context_t *cact, cact_value_t args) \
{ \
    cact_value_t x; \
    if (1 != cact_unpack_args(cact, args, ".", &x)) { \
        return cact_make_error(cact, "Did not get expected number of arguments", args); \
    } \
    PROPAGATE_ERROR(x); \
    return CACT_BOOL_VAL(fn(x)); \
}

#define DEFINE_COMPARISON_BUILTIN(name, fn) \
cact_value_t \
name(cact_context_t *cact, cact_value_t args) \
{ \
    cact_value_t x, y; \
    if (2 != cact_unpack_args(cact, args, "..", &x, &y)) { \
        return cact_make_error(cact, "Did not get expected number of arguments", args); \
    } \
    PROPAGATE_ERROR(x); \
    PROPAGATE_ERROR(y); \
    return CACT_BOOL_VAL(fn(x, y)); \
}


#define CACT_BUILTIN_FUNC(name) \
	cact_value_t name(cact_context_t *, cact_value_t)

CACT_BUILTIN_FUNC(cact_builtin_car);
CACT_BUILTIN_FUNC(cact_builtin_cdr);
CACT_BUILTIN_FUNC(cact_builtin_cons);
CACT_BUILTIN_FUNC(cact_builtin_is_nil);
CACT_BUILTIN_FUNC(cact_builtin_is_pair);
CACT_BUILTIN_FUNC(cact_builtin_is_boolean);
CACT_BUILTIN_FUNC(cact_builtin_not);
CACT_BUILTIN_FUNC(cact_builtin_eq);
CACT_BUILTIN_FUNC(cact_builtin_eqv);
CACT_BUILTIN_FUNC(cact_builtin_equal);
CACT_BUILTIN_FUNC(cact_builtin_display);
CACT_BUILTIN_FUNC(cact_builtin_newline);
CACT_BUILTIN_FUNC(cact_builtin_exit);
CACT_BUILTIN_FUNC(cact_builtin_load);
CACT_BUILTIN_FUNC(cact_builtin_interaction_environment);
CACT_BUILTIN_FUNC(cact_builtin_is_bound);
CACT_BUILTIN_FUNC(cact_builtin_collect_garbage);

// Control structures
CACT_BUILTIN_FUNC(cact_builtin_is_procedure);

/* Errors. */
CACT_BUILTIN_FUNC(cact_builtin_raise);
CACT_BUILTIN_FUNC(cact_builtin_error);
CACT_BUILTIN_FUNC(cact_builtin_is_error_object);
CACT_BUILTIN_FUNC(cact_builtin_error_message);
CACT_BUILTIN_FUNC(cact_builtin_error_irritants);

/* Vectors. */
CACT_BUILTIN_FUNC(cact_builtin_is_vector);
CACT_BUILTIN_FUNC(cact_builtin_make_vector);
CACT_BUILTIN_FUNC(cact_builtin_vector_ref);
CACT_BUILTIN_FUNC(cact_builtin_vector_set);
CACT_BUILTIN_FUNC(cact_builtin_vector_length);

#endif // cact_builtin_h_INCLUDED

