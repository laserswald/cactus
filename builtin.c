#include "env.h"
#include "sexp.h"
#include "sym.h"
#include "eval.h"
#include "globals.h"
#include "debug.h"
#include "utils.h"
#include "config.h"
#include "load.h"
#include "read.h"

/* Unpack arguments, and take the car of the list at the first argument */
cact_val* 
cact_builtin_car(cact_val *x, cact_env *e) 
{
    cact_val* arg = cact_eval(car(x), e);
    PROPAGATE_ERROR(arg);

    if (! arg) {
        return cact_make_error("Cannot take car of null", NULL);
    }

    return car(arg);
}

cact_val* 
cact_builtin_cdr(cact_val *x, cact_env *e) 
{
    cact_val* arg = cact_eval(car(x), e);
    PROPAGATE_ERROR(arg);

    if (! arg) {
        return cact_make_error("Cannot take cdr of null", NULL);
    }

    return cdr(arg);
}

cact_val* 
cact_builtin_cons(cact_val *x, cact_env *e) 
{
    cact_val *fst = cact_eval(car(x), e);
    cact_val *snd = cact_eval(cadr(x), e);
    PROPAGATE_ERROR(fst);
    PROPAGATE_ERROR(snd);
    cact_val *p = cons(fst, snd);
    return p;
}

cact_val* 
cact_builtin_is_nil(cact_val *x, cact_env *e) 
{
    cact_val *arg = cact_eval(car(x), e);
    PROPAGATE_ERROR(arg);
    if (! arg) {
        return cact_make_integer(1);
    }
    return cact_make_integer(0);
}

cact_val* 
cact_builtin_is_pair(cact_val *x, cact_env *e) 
{
    cact_val *arg = cact_eval(car(x), e);
    PROPAGATE_ERROR(arg);
    if (is_pair(arg))
        return cact_make_integer(1);
    else
        return cact_make_integer(0);
}

cact_val* 
cact_builtin_is_number(cact_val *x, cact_env *e)
{
    cact_val *arg = cact_eval(car(x), e);
    PROPAGATE_ERROR(arg);
    if (is_int(arg) || is_float(arg))
        return cact_make_integer(1);
    else
        return cact_make_integer(0);
}

cact_val* 
cact_builtin_eq(cact_val *x, cact_env *e) 
{
    cact_val *fst = cact_eval(car(x), e);
    cact_val *snd = cact_eval(cadr(x), e);
    PROPAGATE_ERROR(fst);
    PROPAGATE_ERROR(snd);
    return cact_make_integer(equals(fst, snd));
}

cact_val* 
cact_builtin_display(cact_val *x, cact_env *e) 
{
    print_sexp(cact_eval(car(x), e));
    return &undefined;
}

cact_val* 
cact_builtin_newline(cact_val *x, cact_env *unused) 
{
    puts("");
    return &undefined;
}

cact_val* 
cact_builtin_progn(cact_val *x, cact_env *e) 
{
    cact_val *result = &undefined;

    if (is_pair(x)) {
        LIST_FOR_EACH(x, pair) {
            result = cact_eval(car(pair), e);
            PROPAGATE_ERROR(result);
        }
    } else {
        result = cact_eval(x, e);
    }

    return result;
}

cact_val* 
cact_builtin_plus(cact_val *x, cact_env *e)
{
    int result = 0;

    LIST_FOR_EACH(x, pair) {
        cact_val* addend = cact_eval(car(pair), e);
        PROPAGATE_ERROR(addend);
        int a = to_int(addend, "+");
        result += a;
    }

    return cact_make_integer(result);
}

cact_val* 
cact_builtin_times(cact_val *x, cact_env *e)
{
    int result = 1;

    LIST_FOR_EACH(x, pair) {
        cact_val* factor = cact_eval(car(pair), e);
        PROPAGATE_ERROR(factor);
        int a = to_int(factor, "*");
        result *= a;
    }

    return cact_make_integer(result);
}

cact_val* 
cact_builtin_minus(cact_val *x, cact_env *e)
{
    int result = to_int(car(x), "-");

    cact_val *rest = cdr(x);

    LIST_FOR_EACH(rest, pair) {
        // Fancy word.
        cact_val* subtrahend = cact_eval(car(pair), e);
        PROPAGATE_ERROR(subtrahend);
        int a = to_int(subtrahend, "-");
        result -= a;
    }

    return cact_make_integer(result);
}

cact_val*
cact_builtin_divide(cact_val *x, cact_env *e)
{
    int result = to_int(car(x), "/");

    cact_val *rest = cdr(x);

    LIST_FOR_EACH(rest, pair) {
        // Fancy word.
        cact_val* divisor= cact_eval(car(pair), e);
        PROPAGATE_ERROR(divisor);
        int a = to_int(divisor, "/");
        if (a == 0) {
            return cact_make_error("Division by zero", NULL);
        }
        result /= a;
    }

    return cact_make_integer(result);
}

cact_val* 
cact_builtin_exit(cact_val *x, cact_env *e)
{
    // Invoke any ending things from dynamic-wind
    // Exit
    exit(0);
    return NULL;
}

cact_val* 
cact_builtin_load(cact_val *x, cact_env *e)
{
    cact_val *fname = cact_eval(car(x), e);
    if (! is_str(fname)) {
        return cact_make_error("`load` expects a string", x);
    }

    printf("LOADING: %s\n", fname->s.str);
    FILE *f = fopen(fname->s.str, "r");
    if (! f) {
        return cact_make_error("load: no file found", fname);
    }

    int result = cact_load(f, e);
    if (result != CACT_READ_OK) {
        return cact_make_error("load: could not read file", fname);
    }

    return &undefined;
}

cact_val* 
cact_builtin_is_boolean(cact_val *x, cact_env *e)
{   
	cact_val *maybe_bool = cact_eval(car(x), e);
    PROPAGATE_ERROR(maybe_bool);
	return cact_make_boolean(is_bool(maybe_bool));
}

cact_val* 
cact_builtin_not(cact_val *x, cact_env *e)
{ 
	cact_val *arg = cact_eval(car(x), e);
    PROPAGATE_ERROR(arg);
	return sexp_not(arg);
}

void 
cact_make_builtin(cact_env *e, cact_val *x, cact_val *(fn)(cact_val*, cact_env*))
{
    cact_val *c = malloc(sizeof(*c));
    c->t = CACT_TYPE_PROCEDURE;
    c->c = malloc(sizeof(cact_proc));
    c->c->nativefn = fn;
    envadd(e, x, c);
}

cact_env* 
cact_make_builtins() 
{
    cact_env *env = malloc(sizeof(cact_env));
    envinit(env, NULL);

    if_sym.sym = "if";
    define_sym.sym = "define";
    begin_sym.sym = "begin";
    quote_sym.sym = "quote";
    lambda_sym.sym = "lambda";
    set_sym.sym = "set!";

    int i;
    for (i = 0; i < LENGTH(builtins); i++) {
        cact_make_builtin(env, cact_make_symbol(builtins[i].name), builtins[i].fn);
    }

    return env;
}
