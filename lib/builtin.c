#include <stdarg.h>

#include "cactus/internal/debug.h"
#include "cactus/internal/utils.h"

#include "cactus/core.h"
#include "cactus/eval.h"
#include "cactus/read.h"
#include "cactus/write.h"

#include "cactus/val.h"
#include "cactus/pair.h"
#include "cactus/num.h"
#include "cactus/str.h"
#include "cactus/err.h"
#include "cactus/bool.h"
#include "cactus/char.h"


bool
unpack_typecheck(const struct cact_val arg, const char c)
{
    switch (c) {
    case '.':
        return true;
    case 'p':
        return cact_is_pair(arg);
    case 'c':
        return cact_is_procedure(arg);
    }
    fprintf(stdout, "cactus: fatal error: could not understand unpack args character '%c'", c);
    abort();
}

int
cact_unpack_args(struct cactus *cact, struct cact_val arglist, const char *format, ...)
{
    va_list slots;
    const char *c;
    int cnt = 0;

    if (! cact_is_pair(arglist)) {
        fprintf(stdout, "cactus: fatal error: expected pair for argument list");
        abort();
    }
    struct cact_val current_pair = arglist;

    c = format;
    va_start(slots, format);

    while (*c != '\0') {
        if (! cact_is_null(current_pair)) {
            struct cact_val current_val = cact_eval(cact, cact_car(cact, current_pair));
            struct cact_val *slot = va_arg(slots, struct cact_val*);

            if (unpack_typecheck(current_val, *c)) {
                *slot = current_val;
            } else {
                *slot = cact_make_error(cact, "type checking error", CACT_NULL_VAL);
            }

        } else {
            struct cact_val *slot = va_arg(slots, struct cact_val*);
            *slot = CACT_NULL_VAL;
        }

        cnt++;
        c++;
        current_pair = cact_cdr(cact, current_pair);
    }

    va_end(slots);

    while (! cact_is_null(current_pair)) {
        cnt++;
        current_pair = cact_cdr(cact, current_pair);
    }

    return cnt;
}

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

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_nil, cact_is_null)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_pair, cact_is_pair)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_fixnum, cact_is_fixnum)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_flonum, cact_is_flonum)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_boolean, cact_is_bool)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_char, cact_is_char)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_number, cact_is_number)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_procedure, cact_is_procedure)

#undef DEFINE_TYPE_PREDICATE_BUILTIN


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

DEFINE_COMPARISON_BUILTIN(cact_builtin_eq, cact_val_eq)
DEFINE_COMPARISON_BUILTIN(cact_builtin_eqv, cact_val_eqv)
DEFINE_COMPARISON_BUILTIN(cact_builtin_equal, cact_val_equal)

#undef DEFINE_COMPARISON_BUILTIN

struct cact_val
cact_builtin_display(struct cactus *cact, struct cact_val args)
{
    struct cact_val x;

    if (1 != cact_unpack_args(cact, args, ".", &x)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(x);

    cact_display(x);

    return CACT_UNDEF_VAL;
}

struct cact_val
cact_builtin_newline(struct cactus *cact, struct cact_val args)
{
    puts("");

    return CACT_UNDEF_VAL;
}

/*
 * Pair builtins.
 */

/* Unpack arguments, and take the car of the list at the first argument */
struct cact_val
cact_builtin_car(struct cactus *cact, struct cact_val args)
{
    struct cact_val l;

    if (1 != cact_unpack_args(cact, args, "p", &l)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(l);

    return cact_car(cact, l);
}

struct cact_val
cact_builtin_cdr(struct cactus *cact, struct cact_val args)
{
    struct cact_val l;

    if (1 != cact_unpack_args(cact, args, "p", &l)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(l);

    return cact_cdr(cact, l);
}

struct cact_val
cact_builtin_cons(struct cactus *cact, struct cact_val args)
{
    struct cact_val a, d;

    if (2 != cact_unpack_args(cact, args, "..", &a, &d)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(a);
    PROPAGATE_ERROR(d);

    return cact_cons(cact, a, d);
}

struct cact_val
cact_builtin_plus(struct cactus *cact, struct cact_val args)
{
    int result = 0;

    CACT_LIST_FOR_EACH_ITEM(cact, addend_expr, args) {
        struct cact_val addend = cact_eval(cact, addend_expr);
        PROPAGATE_ERROR(addend);
        int a = cact_to_long(addend, "+");
        result += a;
    }

    return CACT_FIX_VAL(result);
}

struct cact_val
cact_builtin_times(struct cactus *cact, struct cact_val args)
{
    int result = 1;

    CACT_LIST_FOR_EACH_ITEM(cact, factor_expr, args) {
        struct cact_val factor = cact_eval(cact, factor_expr);
        PROPAGATE_ERROR(factor);
        int a = cact_to_long(factor, "*");
        result *= a;
    }

    return CACT_FIX_VAL(result);
}

struct cact_val
cact_builtin_minus(struct cactus *cact, struct cact_val x)
{
    struct cact_val initial, rest, subtrahend;
    int result, a;

    initial = cact_eval(cact, cact_car(cact, x));
    PROPAGATE_ERROR(initial);

    result = cact_to_long(initial, "-");
    rest = cact_cdr(cact, x);

    // Fancy word.
    CACT_LIST_FOR_EACH_ITEM(cact, subtrahend_expr, rest) {
        subtrahend = cact_eval(cact, subtrahend_expr);
        PROPAGATE_ERROR(subtrahend);
        a = cact_to_long(subtrahend, "-");
        result -= a;
    }

    return CACT_FIX_VAL(result);
}

struct cact_val
cact_builtin_divide(struct cactus *cact, struct cact_val x)
{
    int result = cact_to_long(cact_car(cact, x), "/");

    struct cact_val rest = cact_cdr(cact, x);

    CACT_LIST_FOR_EACH_ITEM(cact, divisor_expr, rest) {
        // Fancy word.
        struct cact_val divisor = cact_eval(cact, divisor_expr);
        PROPAGATE_ERROR(divisor);
        int a = cact_to_long(divisor, "/");
        if (a == 0) {
            return cact_make_error(cact, "Division by zero", CACT_NULL_VAL);
        }
        result /= a;
    }

    return CACT_FIX_VAL(result);
}

struct cact_val
cact_builtin_exit(struct cactus *cact, struct cact_val x)
{
    // Invoke any ending things from dynamic-wind
    // Exit
    exit(0);
    return CACT_UNDEF_VAL;
}

struct cact_val
cact_builtin_load(struct cactus *cact, struct cact_val x)
{
    struct cact_val fname = cact_eval(cact, cact_car(cact, x));
    if (! cact_is_string(fname)) {
        return cact_make_error(cact, "`load` expects a string", x);
    }

    FILE *f = fopen(cact_to_string(fname, "load")->str, "r");
    if (! f) {
        return cact_make_error(cact, "load: no file found", fname);
    }

    // Need to save the current reader state
    struct cact_lexer prev_lexer = cact->lexer;

    struct cact_val result = cact_eval_file(cact, f);
    if (cact_is_error(result)) {
        cact_fdisplay(stderr, result);
        return cact_make_error(cact, "load: could not read file", fname);
    }

    cact->lexer = prev_lexer;

    return CACT_UNDEF_VAL;
}

struct cact_val
cact_builtin_not(struct cactus *cact, struct cact_val x)
{
    struct cact_val arg = cact_eval(cact, cact_car(cact, x));
    PROPAGATE_ERROR(arg);

    return cact_bool_not(arg);
}

struct cact_val
cact_builtin_interaction_environment(struct cactus *cact, struct cact_val x)
{
    return CACT_OBJ_VAL((struct cact_obj *) cact->root_env);
}

struct cact_val
cact_builtin_collect_garbage(struct cactus *cact, struct cact_val x)
{
    cact_collect_garbage(cact);
    return CACT_UNDEF_VAL;
}

struct cact_val
cact_builtin_error(struct cactus *cact, struct cact_val x)
{
	return cact_make_error(cact, cact_to_string(cact_car(cact, x), "error")->str, cact_cdr(cact, x));
}

struct cact_val
cact_builtin_is_bound(struct cactus *cact, struct cact_val x)
{
	struct cact_val arg = cact_eval(cact, cact_car(cact, x));
	return CACT_BOOL_VAL(cact_env_is_bound(cact_current_env(cact), cact_to_symbol(arg, "bound?")));
}

struct cact_val
cact_builtin_with_exception_handler(struct cactus *cact, struct cact_val args)
{
    struct cact_val handler, thunk;

    if (2 != cact_unpack_args(cact, args, "cc", &handler, &thunk)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    cact_current_cont(cact)->exn_handler = cact_to_procedure(handler, "with-exception-handler");

    return cact_proc_apply(
        cact, 
        cact_to_procedure(thunk, "with-exception-handler"), 
        CACT_NULL_VAL
    );
}

