#include <stdarg.h>

#include "builtin.h"

#include "internal/debug.h"
#include "internal/utils.h"

#include "core.h"
#include "evaluator/eval.h"
#include "read.h"
#include "write.h"

#include "val.h"
#include "pair.h"
#include "num.h"
#include "str.h"
#include "err.h"
#include "bool.h"
#include "char.h"
#include "vec.h"

/**
 * Check the argument with the type character.
 */
static bool
unpack_typecheck(const struct cact_val arg, const char c)
{
    switch (c) {
    case '.':
        return true;
    case 'p':
        return cact_is_pair(arg);
    case 'c': // for closure
        return cact_is_procedure(arg);
    case 'v':
        return cact_is_vector(arg);
    case 'i':
        return cact_is_fixnum(arg);
    }
    fprintf(stdout, "cactus: fatal error: could not understand unpack args character '%c'", c);
    abort();
}

/**
 * Check all the arguments in the argument list and
 */
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
            struct cact_val current_val = cact_car(cact, current_pair);
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

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_nil, cact_is_null)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_pair, cact_is_pair)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_boolean, cact_is_bool)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_char, cact_is_char)
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_procedure, cact_is_procedure)

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
    struct cact_val fname = cact_car(cact, x);
    if (! cact_is_string(fname)) {
        return cact_make_error(cact, "`load` expects a string", x);
    }

    FILE *f = fopen(cact_to_string(fname, "load")->str, "r");
    if (! f) {
        return cact_make_error(cact, "load: no file found", fname);
    }

    // Need to save the current reader state
    struct cact_lexer prev_lexer = cact->lexer;

    struct cact_cont *nc = (struct cact_cont*)cact_alloc(cact, CACT_OBJ_CONT);
    cact_cont_init(nc, cact_current_env(cact), NULL);
    cact_call_stack_push(cact, nc);

    struct cact_val result = cact_eval_file(cact, f);
    if (cact_is_error(result)) {
        cact_fdisplay(stderr, result);
        return cact_make_error(cact, "load: could not read file", fname);
    }

    cact_call_stack_pop(cact);
    cact->lexer = prev_lexer;

    return CACT_UNDEF_VAL;
}

struct cact_val
cact_builtin_not(struct cactus *cact, struct cact_val x)
{
    struct cact_val arg = cact_car(cact, x);

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
    struct cact_val arg = cact_car(cact, x);

    return CACT_BOOL_VAL(cact_env_is_bound(cact_current_env(cact), cact_to_symbol(arg, "bound?")));
}

struct cact_val
cact_builtin_with_exception_handler(struct cactus *cact, struct cact_val args)
{
    struct cact_val handler, thunk;

    if (2 != cact_unpack_args(cact, args, "cc", &handler, &thunk)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    cact_current_cont(cact)->exn_handler = cact_to_procedure(
            handler, "with-exception-handler"
                                           );

    return cact_proc_apply(
               cact,
               cact_to_procedure(thunk, "with-exception-handler"),
               CACT_NULL_VAL
           );
}

