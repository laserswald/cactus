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
Sexp* 
builtin_car(Sexp *x, Env *e) 
{
    Sexp* arg = eval(car(x), e);
    PROPAGATE_ERROR(arg);

    if (! arg) {
        return make_error("Cannot take car of null", NULL);
    }

    return car(arg);
}

Sexp* 
builtin_cdr(Sexp *x, Env *e) 
{
    Sexp* arg = eval(car(x), e);
    PROPAGATE_ERROR(arg);

    if (! arg) {
        return make_error("Cannot take cdr of null", NULL);
    }

    return cdr(arg);
}

Sexp* 
builtin_cons(Sexp *x, Env *e) 
{
    Sexp *fst = eval(car(x), e);
    Sexp *snd = eval(cadr(x), e);
    PROPAGATE_ERROR(fst);
    PROPAGATE_ERROR(snd);
    Sexp *p = cons(fst, snd);
    return p;
}

Sexp* 
builtin_is_nil(Sexp *x, Env *e) 
{
    Sexp *arg = eval(car(x), e);
    PROPAGATE_ERROR(arg);
    if (! arg) {
        return make_integer(1);
    }
    return make_integer(0);
}

Sexp* 
builtin_is_pair(Sexp *x, Env *e) 
{
    Sexp *arg = eval(car(x), e);
    PROPAGATE_ERROR(arg);
    if (is_pair(arg))
        return make_integer(1);
    else
        return make_integer(0);
}

Sexp* 
builtin_is_number(Sexp *x, Env *e)
{
    Sexp *arg = eval(car(x), e);
    PROPAGATE_ERROR(arg);
    if (is_int(arg) || is_float(arg))
        return make_integer(1);
    else
        return make_integer(0);
}

Sexp* 
builtin_eq(Sexp *x, Env *e) 
{
    Sexp *fst = eval(car(x), e);
    Sexp *snd = eval(cadr(x), e);
    PROPAGATE_ERROR(fst);
    PROPAGATE_ERROR(snd);
    return make_integer(equals(fst, snd));
}

Sexp* 
builtin_display(Sexp *x, Env *e) 
{
    print_sexp(eval(car(x), e));
    return &undefined;
}

Sexp* 
builtin_newline(Sexp *x, Env *unused) 
{
    puts("");
    return &undefined;
}

Sexp* 
builtin_progn(Sexp *x, Env *e) 
{
    Sexp *result = &undefined;

    if (is_pair(x)) {
        LIST_FOR_EACH(x, pair) {
            result = eval(car(pair), e);
            PROPAGATE_ERROR(result);
        }
    } else {
        result = eval(x, e);
    }

    return result;
}

Sexp* 
builtin_plus(Sexp *x, Env *e)
{
    int result = 0;

    LIST_FOR_EACH(x, pair) {
        Sexp* addend = eval(car(pair), e);
        PROPAGATE_ERROR(addend);
        int a = to_int(addend, "+");
        result += a;
    }

    return make_integer(result);
}

Sexp* 
builtin_times(Sexp *x, Env *e)
{
    int result = 1;

    LIST_FOR_EACH(x, pair) {
        Sexp* factor = eval(car(pair), e);
        PROPAGATE_ERROR(factor);
        int a = to_int(factor, "*");
        result *= a;
    }

    return make_integer(result);
}

Sexp* 
builtin_minus(Sexp *x, Env *e)
{
    int result = to_int(car(x), "-");

    Sexp *rest = cdr(x);

    LIST_FOR_EACH(rest, pair) {
        // Fancy word.
        Sexp* subtrahend = eval(car(pair), e);
        PROPAGATE_ERROR(subtrahend);
        int a = to_int(subtrahend, "-");
        result -= a;
    }

    return make_integer(result);
}

Sexp*
builtin_divide(Sexp *x, Env *e)
{
    int result = to_int(car(x), "/");

    Sexp *rest = cdr(x);

    LIST_FOR_EACH(rest, pair) {
        // Fancy word.
        Sexp* divisor= eval(car(pair), e);
        PROPAGATE_ERROR(divisor);
        int a = to_int(divisor, "/");
        if (a == 0) {
            return make_error("Division by zero", NULL);
        }
        result /= a;
    }

    return make_integer(result);
}

Sexp* 
builtin_exit(Sexp *x, Env *e)
{
    // Invoke any ending things from dynamic-wind
    // Exit
    exit(0);
    return NULL;
}

Sexp* 
builtin_load(Sexp *x, Env *e)
{
    Sexp *fname = eval(car(x), e);
    if (! is_str(fname)) {
        return make_error("`load` expects a string", x);
    }

    printf("LOADING: %s\n", fname->s.str);
    FILE *f = fopen(fname->s.str, "r");
    if (! f) {
        return make_error("load: no file found", fname);
    }

    int result = runfile(f, e);
    if (result != READSEXP_OK) {
        return make_error("load: could not read file", fname);
    }

    return &undefined;
}

Sexp* 
builtin_is_boolean(Sexp *x, Env *e)
{   
	Sexp *maybe_bool = eval(car(x), e);
    PROPAGATE_ERROR(maybe_bool);
	return make_boolean(is_bool(maybe_bool));
}

Sexp* 
builtin_not(Sexp *x, Env *e)
{ 
	Sexp *arg = eval(car(x), e);
    PROPAGATE_ERROR(arg);
	return sexp_not(arg);
}

void 
make_builtin(Env *e, Sexp *x, Sexp *(fn)(Sexp*, Env*))
{
    Sexp *c = malloc(sizeof(*c));
    c->t = TYPE_CLOSURE;
    c->c = malloc(sizeof(Closure));
    c->c->nativefn = fn;
    envadd(e, x, c);
}

Env* 
make_builtins() 
{
    Env *env = malloc(sizeof(Env));
    envinit(env, NULL);

    if_sym.sym = "if";
    define_sym.sym = "define";
    begin_sym.sym = "begin";
    quote_sym.sym = "quote";
    lambda_sym.sym = "lambda";
    set_sym.sym = "set!";

    int i;
    for (i = 0; i < LENGTH(builtins); i++) {
        make_builtin(env, make_symbol(builtins[i].name), builtins[i].fn);
    }

    return env;
}
