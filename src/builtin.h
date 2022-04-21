#ifndef cact_builtin_h_INCLUDED
#define cact_builtin_h_INCLUDED

#include "proc.h"
#include "err.h"

/**
 * Check all the arguments in the argument list and
 */
int
cact_unpack_args(struct cactus *cact, struct cact_val arglist, const char *format, ...);

/*
 * A structure for quickly adding new native functions.
 */

struct cact_builtin {
	char* name;
	cact_native_func fn;
};

/* Define a native procedure in the global default namespace */
void cact_define_builtin(struct cactus *, const char *, cact_native_func);

/* Define a set of procedures in the global default namespace */
void cact_define_builtin_array(struct cactus *cact, struct cact_builtin *builtins, size_t len);

#define DEFINE_TYPE_PREDICATE_BUILTIN(name, fn) \
struct cact_val \
name(struct cactus *cact, struct cact_val args) \
{ \
    struct cact_val x; \
    if (1 != cact_unpack_args(cact, args, ".", &x)) { \
        return cact_make_error(cact, "Did not get expected number of arguments", args); \
    } \
    PROPAGATE_ERROR(x); \
    return CACT_BOOL_VAL(fn(x)); \
}

#define DEFINE_COMPARISON_BUILTIN(name, fn) \
struct cact_val \
name(struct cactus *cact, struct cact_val args) \
{ \
    struct cact_val x, y; \
    if (2 != cact_unpack_args(cact, args, "..", &x, &y)) { \
        return cact_make_error(cact, "Did not get expected number of arguments", args); \
    } \
    PROPAGATE_ERROR(x); \
    PROPAGATE_ERROR(y); \
    return CACT_BOOL_VAL(fn(x, y)); \
}


#define BUILTIN_FUNC(name) \
	struct cact_val name(struct cactus *, struct cact_val)

BUILTIN_FUNC(cact_builtin_car);
BUILTIN_FUNC(cact_builtin_cdr);
BUILTIN_FUNC(cact_builtin_cons);
BUILTIN_FUNC(cact_builtin_is_nil);
BUILTIN_FUNC(cact_builtin_is_pair);

BUILTIN_FUNC(cact_builtin_is_boolean);
BUILTIN_FUNC(cact_builtin_not);
BUILTIN_FUNC(cact_builtin_eq);
BUILTIN_FUNC(cact_builtin_eqv);
BUILTIN_FUNC(cact_builtin_equal);

BUILTIN_FUNC(cact_builtin_exit);
BUILTIN_FUNC(cact_builtin_load);
BUILTIN_FUNC(cact_builtin_interaction_environment);
BUILTIN_FUNC(cact_builtin_is_bound);
BUILTIN_FUNC(cact_builtin_collect_garbage);

// Control structures
BUILTIN_FUNC(cact_builtin_is_procedure);

/* Errors. */
BUILTIN_FUNC(cact_builtin_raise);
BUILTIN_FUNC(cact_builtin_error);
BUILTIN_FUNC(cact_builtin_is_error_object);
BUILTIN_FUNC(cact_builtin_error_message);
BUILTIN_FUNC(cact_builtin_error_irritants);

/* Vectors. */
BUILTIN_FUNC(cact_builtin_is_vector);
BUILTIN_FUNC(cact_builtin_make_vector);
BUILTIN_FUNC(cact_builtin_vector_ref);
BUILTIN_FUNC(cact_builtin_vector_set);
BUILTIN_FUNC(cact_builtin_vector_length);

/* Input and output */
BUILTIN_FUNC(cact_builtin_open_input_file);
BUILTIN_FUNC(cact_builtin_is_input_port);
BUILTIN_FUNC(cact_builtin_read_char);
BUILTIN_FUNC(cact_builtin_close_input_port);
BUILTIN_FUNC(cact_builtin_open_output_file);
BUILTIN_FUNC(cact_builtin_is_output_port);
BUILTIN_FUNC(cact_builtin_write_char);
BUILTIN_FUNC(cact_builtin_close_output_port);
BUILTIN_FUNC(cact_builtin_display);
BUILTIN_FUNC(cact_builtin_newline);

#endif // cact_builtin_h_INCLUDED

