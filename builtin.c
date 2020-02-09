#include "core.h"
#include "env.h"
#include "sexp.h"
#include "sym.h"
#include "eval.h"
#include "globals.h"
#include "debug.h"
#include "utils.h"
// #include "config.h"
#include "read.h"

/* Unpack arguments, and take the car of the list at the first argument */
struct cact_val *cact_builtin_car(struct cactus *cact, struct cact_val *args) 
{
    struct cact_val* arg = cact_eval(cact, car(args));
    PROPAGATE_ERROR(arg);

    if (! arg) {
        return cact_make_error("Cannot take car of null", NULL);
    }

    return car(arg);
}

struct cact_val* cact_builtin_cdr(struct cactus *cact, struct cact_val *x) 
{
    struct cact_val* arg = cact_eval(cact, car(x));
    PROPAGATE_ERROR(arg);

    if (! arg) {
        return cact_make_error("Cannot take cdr of null", NULL);
    }

    return cdr(arg);
}

struct cact_val *cact_builtin_cons(struct cactus *cact, struct cact_val *x) 
{
    struct cact_val *fst = cact_eval(cact, car(x));
    struct cact_val *snd = cact_eval(cact, cadr(x));
    PROPAGATE_ERROR(fst);
    PROPAGATE_ERROR(snd);
    struct cact_val *p = cons(fst, snd);
    return p;
}

struct cact_val* cact_builtin_is_nil(struct cactus *cact, struct cact_val *x) 
{
    struct cact_val *arg = cact_eval(cact, car(x));
    PROPAGATE_ERROR(arg);
    if (! arg) {
        return cact_make_boolean(true);
    }
    return cact_make_boolean(false);
}

struct cact_val* cact_builtin_is_pair(struct cactus *cact, struct cact_val *x) 
{
    struct cact_val *arg = cact_eval(cact, car(x));
    PROPAGATE_ERROR(arg);
    if (is_pair(arg))
        return cact_make_boolean(true);
    else
        return cact_make_boolean(false);
}

struct cact_val* cact_builtin_is_number(struct cactus *cact, struct cact_val *x)
{
    struct cact_val *arg = cact_eval(cact, car(x));
    PROPAGATE_ERROR(arg);
    if (is_int(arg) || is_float(arg))
        return cact_make_boolean(true);
    else
        return cact_make_boolean(false);
}

struct cact_val* cact_builtin_eq(struct cactus *cact, struct cact_val *x) 
{
    struct cact_val *fst = cact_eval(cact, car(x));
    struct cact_val *snd = cact_eval(cact, cadr(x));
    PROPAGATE_ERROR(fst);
    PROPAGATE_ERROR(snd);
    return cact_make_boolean(equals(fst, snd));
}

struct cact_val* cact_builtin_display(struct cactus *cact, struct cact_val *x) 
{
    print_sexp(cact_eval(cact, car(x)));
    return &undefined;
}

struct cact_val* cact_builtin_newline(struct cactus *cact, struct cact_val *x) 
{
    puts("");
    return &undefined;
}

struct cact_val* cact_builtin_begin(struct cactus *cact, struct cact_val *x) 
{
    struct cact_val *result = &undefined;

    if (is_pair(x)) {
        LIST_FOR_EACH(x, pair) {
            result = cact_eval(cact, car(pair));
            PROPAGATE_ERROR(result);
        }
    } else {
        result = cact_eval(cact, x);
    }

    return result;
}

struct cact_val* cact_builtin_plus(struct cactus *cact, struct cact_val *x)
{
    int result = 0;

    LIST_FOR_EACH(x, pair) {
        struct cact_val* addend = cact_eval(cact, car(pair));
        PROPAGATE_ERROR(addend);
        int a = to_int(addend, "+");
        result += a;
    }

    return cact_make_integer(result);
}

struct cact_val* cact_builtin_times(struct cactus *cact, struct cact_val *x)
{
    int result = 1;

    LIST_FOR_EACH(x, pair) {
        struct cact_val* factor = cact_eval(cact, car(pair));
        PROPAGATE_ERROR(factor);
        int a = to_int(factor, "*");
        result *= a;
    }

    return cact_make_integer(result);
}

struct cact_val* cact_builtin_minus(struct cactus *cact, struct cact_val *x)
{
    int result = to_int(car(x), "-");

    struct cact_val *rest = cdr(x);

    LIST_FOR_EACH(rest, pair) {
        // Fancy word.
        struct cact_val* subtrahend = cact_eval(cact, car(pair));
        PROPAGATE_ERROR(subtrahend);
        int a = to_int(subtrahend, "-");
        result -= a;
    }

    return cact_make_integer(result);
}

struct cact_val* cact_builtin_divide(struct cactus *cact, struct cact_val *x)
{
    int result = to_int(car(x), "/");

    struct cact_val *rest = cdr(x);

    LIST_FOR_EACH(rest, pair) {
        // Fancy word.
        struct cact_val* divisor = cact_eval(cact, car(pair));
        PROPAGATE_ERROR(divisor);
        int a = to_int(divisor, "/");
        if (a == 0) {
            return cact_make_error("Division by zero", NULL);
        }
        result /= a;
    }

    return cact_make_integer(result);
}

struct cact_val* cact_builtin_exit(struct cactus *cact, struct cact_val *x)
{
    // Invoke any ending things from dynamic-wind
    // Exit
    exit(0);
    return NULL;
}

struct cact_val* cact_builtin_load(struct cactus *cact, struct cact_val *x)
{
    struct cact_val *fname = cact_eval(cact, car(x));
    if (! is_str(fname)) {
        return cact_make_error("`load` expects a string", x);
    }

    printf("LOADING: %s\n", fname->s.str);
    FILE *f = fopen(fname->s.str, "r");
    if (! f) {
        return cact_make_error("load: no file found", fname);
    }

    struct cact_val *result = cact_eval_file(cact, f);
    if (is_error(result)) {
        return cact_make_error("load: could not read file", fname);
    }

    return &undefined;
}

struct cact_val* cact_builtin_is_boolean(struct cactus *cact, struct cact_val *x)
{   
	struct cact_val *maybe_bool = cact_eval(cact, car(x));
    PROPAGATE_ERROR(maybe_bool);
	return cact_make_boolean(is_bool(maybe_bool));
}

struct cact_val* cact_builtin_not(struct cactus *cact, struct cact_val *x)
{ 
	struct cact_val *arg = cact_eval(cact, car(x));
    PROPAGATE_ERROR(arg);
	return sexp_not(arg);
}

