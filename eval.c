#include "debug.h"
#include "sexp.h"
#include "sym.h"
#include "env.h"
#include "eval.h"
#include "proc.h"
#include "builtin.h"
#include "globals.h"
#include "utils.h"
#include "write.h"

struct cact_val *special_quote(struct cactus *cact, struct cact_val *args)
{
    return args;
}

bool is_truthy(struct cact_val *x)
{
    if (is_bool(x) && x->b == false) {
        return false;
    }

    return true;
}

struct cact_val *special_if(struct cactus *cact, struct cact_val *args)
{
    cact_val* cond = car(args);
    cact_val* cseq = cadr(args);
    cact_val* alt = caddr(args);
    cact_val* result = &undefined;

    if (is_truthy(cact_eval(cact, cond))) {
        result = cact_eval(cact, cseq);
    } else {
        result = cact_eval(cact, alt);
    }

    return result;
}

struct cact_val *special_define(struct cactus *cact, struct cact_val *args)
{
    cact_val *term = car(args);
    cact_val *defn = car(cdr(args));

    cact_val *value;

    if (is_sym(term)) {
        value = cact_eval(cact, defn);
    } else if (is_pair(term)) {
        cact_val *params = cdr(term);
        term = car(term);
        value = cact_make_procedure(cact->current_env, params, defn);
    }

    if (envadd(cact->current_env, term, value) < 0) {
        return cact_make_error("Could not create definition: definition already exists", term);
    }

    return &undefined;
}

struct cact_val* special_lambda(struct cactus *cact, struct cact_val *args)
{
    cact_val *lambda_args = car(args);
    cact_val *body = cdr(args);
    return cact_make_procedure(cact->current_env, lambda_args, body);
}

struct cact_val* special_begin(struct cactus *cact, struct cact_val *args)
{
    struct cact_val *result = &undefined;

    if (is_pair(args)) {
        LIST_FOR_EACH(args, pair) {
            result = cact_eval(cact, car(pair));
            PROPAGATE_ERROR(result);
        }
    } else {
        result = cact_eval(cact, args);
    }

    return result;
}

struct cact_val* special_set_bang(struct cactus *cact, struct cact_val *args)
{
    struct cact_val *term = car(args);
    struct cact_val *defn = car(cdr(args));
    struct cact_val *value = cact_eval(cact, defn);

    if (envset(cact->current_env, term, value) < 0) {
        return cact_make_error("Could not assign value: no such variable", term);
    }

    return &undefined;
}

struct specials_table {
    cact_symbol *sym;
    struct cact_val *(*fn)(struct cactus *, struct cact_val *);
} specials[] = {
    {&quote_sym,  special_quote},
    {&if_sym,     special_if},
    {&define_sym, special_define},
    {&lambda_sym, special_lambda},
    {&set_sym,    special_set_bang},
    {&begin_sym,  special_begin}
};

/* Evaluate an expression according to an environment. */
struct cact_val *cact_eval(struct cactus *cact, struct cact_val *x)
{
    if (!x) {
        return NULL;
    }

    switch (x->t) {
    case CACT_TYPE_INT:
    case CACT_TYPE_BOOLEAN:
    case CACT_TYPE_DOUBLE:
    case CACT_TYPE_STRING:
    case CACT_TYPE_PROCEDURE:
    case CACT_TYPE_ENVIRONMENT:
        DBG("Evaluating self-cact_evaluating atom %s\n", show_type(x->t));
        return x;

    case CACT_TYPE_SYMBOL: {
        cact_val *found = envlookup(cact->current_env, x);
        if (!found) {
            print_env(cact->current_env);
            return cact_make_error("No such symbol", x);
        }
        return cdr(found);
    }

    case CACT_TYPE_PAIR: {
        struct cact_val *operator = car(x);
        struct cact_val *operands = cdr(x);

        if (! operator) {
            return cact_make_error("Cannot evaluate null as operation", x);
        }

        // If it's a symbol, check if it's special and do the thing
        if (is_sym(operator)) {
            cact_symbol sym = to_sym(operator, "cact_eval");
            int i;
            for (i = 0; i < LENGTH(specials); i++) {
                if (symcmp(specials[i].sym, &sym) == 0) {
                    return specials[i].fn(cact, operands);
                }
            }
            // fallthrough when not a special form
        }

        cact_val *maybe_procedure = cact_eval(cact, operator);

        if (is_error(maybe_procedure)) {
            return maybe_procedure;
        }

        if (! is_procedure(maybe_procedure)) {
            fprintf(stdout, "Cannot apply non-operation %s:\n", show_type(maybe_procedure->t));
            print_sexp(maybe_procedure);
            abort();
        }

        return cact_proc_apply(cact, maybe_procedure->c, operands);
    }

    case CACT_TYPE_ERROR: {
        fprintf(stderr, "Error! %s", x->x.msg);
        print_list(x->x.ctx);
        fprintf(stderr, "\n");
        abort();
    }

    default:
        fprintf(stderr, "Cannot cact_evaluate, got type %s.\n", show_type(x->t));
        abort();
    }
}
