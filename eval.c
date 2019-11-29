#include "debug.h"
#include "sexp.h"
#include "sym.h"
#include "env.h"
#include "eval.h"
#include "builtin.h"
#include "globals.h"
#include "utils.h"
#include "write.h"

/* Apply a closure given the arguments and the environment. */
Sexp *
apply(Closure *clo, Sexp *args, Env *e)
{
    if (clo->nativefn) {
        return clo->nativefn(args, e);
    } else {
        Env params_env = {0};
        envinit(&params_env, clo->env);
        Sexp *current_arg = args;
        LIST_FOR_EACH(clo->argl, param) {
            if (! current_arg) {
                fprintf(stderr, "Not enough arguments!");
                fprint_sexp(stderr, args);
                abort();
            }
            envadd(&params_env, car(param), eval(car(current_arg), e));
            current_arg = cdr(current_arg);
        }
        return builtin_progn(clo->body, &params_env);
    }
}

Sexp* special_quote(Sexp* args, Env* e)
{
    return args;
}

bool is_truthy(Sexp *x)
{
    if (is_bool(x) && x->b == false) {
        return false;
    }

    return true;
}

Sexp* special_if(Sexp* args, Env* e)
{
    Sexp* cond = car(args);
    Sexp* cseq = cadr(args);
    Sexp* alt = caddr(args);
    Sexp* result = &undefined;

    if (is_truthy(eval(cond, e))) {
        result = eval(cseq, e);
    } else {
        result = eval(alt, e);
    }

    return result;
}

Sexp* special_define(Sexp* args, Env* e)
{
    Sexp *term = car(args);
    Sexp *defn = car(cdr(args));

    Sexp *value;

    if (is_sym(term)) {
        value = eval(defn, e);
    } else if (is_pair(term)) {
        Sexp* params = cdr(term);
        term = car(term);
        value = make_closure(e, params, defn);
    }

    if (envadd(e, term, value) < 0) {
        return make_error("Could not create definition: definition already exists", term);
    }

    return &undefined;
}

Sexp* special_lambda(Sexp* operands, Env* e)
{
    Sexp *args = car(operands);
    Sexp *body = cdr(operands);
    return make_closure(e, args, body);
}

Sexp* special_begin(Sexp* args, Env* e)
{
    Sexp *result = &undefined;

    if (is_pair(args)) {
        LIST_FOR_EACH(args, pair) {
            result = eval(car(pair), e);
            PROPAGATE_ERROR(result);
        }
    } else {
        result = eval(args, e);
    }

    return result;
}

Sexp* special_set_bang(Sexp* args, Env* e)
{
    Sexp *term = car(args);
    Sexp *defn = car(cdr(args));
    Sexp *value = eval(defn, e);

    if (envset(e, term, value) < 0) {
        return make_error("Could not assign value: no such variable", term);
    }

    return &undefined;
}

struct specials_table {
    Symbol *sym;
    Sexp* (*fn)(Sexp*, Env*);
} specials[] = {
    {&quote_sym,  special_quote},
    {&if_sym,     special_if},
    {&define_sym, special_define},
    {&lambda_sym, special_lambda},
    {&set_sym,    special_set_bang},
    {&begin_sym,  special_begin}
};

/* Evaluate an expression according to an environment. */
Sexp *
eval(Sexp *x, Env *e)
{
    if (!x) {
        return NULL;
    }

    switch (x->t) {
    case TYPE_INT:
    case TYPE_BOOLEAN:
    case TYPE_DOUBLE:
    case TYPE_STRING:
    case TYPE_CLOSURE:
    case TYPE_ENVIRONMENT:
        DBG("Evaluating self-evaluating atom %s\n", show_type(x->t));
        return x;

    case TYPE_SYMBOL: {
        Sexp *found = envlookup(e, x);
        if (!found) {
            print_env(e);
            return make_error("No such symbol", x);
        }
        return cdr(found);
    }

    case TYPE_PAIR: {
        Sexp *operator = car(x);
        Sexp *operands = cdr(x);

        if (! operator) {
            return make_error("Cannot evaluate null as operation", x);
        }

        // If it's a symbol, check if it's special and do the thing
        if (is_sym(operator)) {
            Symbol sym = to_sym(operator, "eval");
            int i;
            for (i = 0; i < LENGTH(specials); i++) {
                if (symcmp(specials[i].sym, &sym) == 0) {
                    return specials[i].fn(operands, e);
                }
            }
            // fallthrough when not a special form
        }

        Sexp *maybe_closure = eval(operator, e);

        if (is_error(maybe_closure)) {
            return maybe_closure;
        }

        if (! is_closure(maybe_closure)) {
            fprintf(stdout, "Cannot apply non-operation %s:\n", show_type(maybe_closure->t));
            print_sexp(maybe_closure);
            abort();
        }

        return apply(maybe_closure->c, operands, e);
    }

    case TYPE_ERROR: {
        fprintf(stderr, "Error! %s", x->x.msg);
        print_list(x->x.ctx);
        fprintf(stderr, "\n");
        abort();
    }

    default:
        fprintf(stderr, "Cannot evaluate, got type %s.\n", show_type(x->t));
        abort();
    }
}
