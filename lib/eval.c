
#include "cactus/eval.h"

#include "cactus/core.h"
#include "cactus/write.h"

#include "cactus/val.h"
#include "cactus/sym.h"
#include "cactus/str.h"
#include "cactus/env.h"
#include "cactus/proc.h"
#include "cactus/bool.h"
#include "cactus/num.h"
#include "cactus/err.h"

#include "cactus/internal/debug.h"
#include "cactus/internal/utils.h"

struct cact_val 
special_quote(struct cactus *cact, struct cact_val args)
{
    return args;
}

struct cact_val 
special_if(struct cactus *cact, struct cact_val args)
{
    struct cact_val cond = cact_car(cact, args);
    struct cact_val cseq = cact_cadr(cact, args);
    struct cact_val alt = cact_caddr(cact, args);
    struct cact_val result = CACT_UNDEF_VAL;

    if (cact_is_truthy(cact_eval(cact, cond))) {
        result = cact_eval(cact, cseq);
    } else {
        result = cact_eval(cact, alt);
    }

    return result;
}

struct cact_val
special_define(struct cactus *cact, struct cact_val args)
{
    struct cact_val term = cact_car(cact, args);
    struct cact_val defn = cact_cdr(cact, args);

    struct cact_val value;

    if (cact_is_symbol(term)) {
        value = cact_eval(cact, defn);
    } else if (cact_is_pair(term)) {
        struct cact_val params = cact_cdr(cact, term);
        term = cact_car(cact, term);
        value = cact_make_procedure(cact, cact->current_env, params, defn);
    }

    if (cact_env_define(cact, cact->current_env, term, value) < 0) {
        return cact_make_error(cact, "Could not create definition: definition already exists", term);
    }

    return CACT_UNDEF_VAL;
}

struct cact_val 
special_lambda(struct cactus *cact, struct cact_val args)
{
    struct cact_val lambda_args = cact_car(cact, args);
    struct cact_val body = cact_cdr(cact, args);
    return cact_make_procedure(cact, cact->current_env, lambda_args, body);
}

struct cact_val
special_begin(struct cactus *cact, struct cact_val args)
{
    struct cact_val result = CACT_UNDEF_VAL;

    if (cact_is_pair(args)) {
        CACT_LIST_FOR_EACH_ITEM(cact, expr, args) {
            result = cact_eval(cact, expr);
            PROPAGATE_ERROR(result);
        }
    } else {
        result = cact_eval(cact, args);
    }

    return result;
}

struct cact_val
special_set_bang(struct cactus *cact, struct cact_val args)
{
    struct cact_val term = cact_car(cact, args);
    struct cact_val defn = cact_cadr(cact, args);
    struct cact_val value = cact_eval(cact, defn);

    if (cact_env_set(cact, cact->current_env, term, value) < 0) {
        return cact_make_error(cact, "Could not assign value: no such variable", term);
    }

    return CACT_UNDEF_VAL;
}

static bool 
is_self_evaluating(struct cact_val x)
{
    return cact_is_null(x) 
        || cact_is_bool(x)
        || cact_is_number(x)
        || cact_is_procedure(x)
        || cact_is_string(x)
        || cact_is_env(x);
}

/* Evaluate an expression. */
struct cact_val
cact_eval(struct cactus *cact, struct cact_val x)
{
    if (is_self_evaluating(x)) {
        return x;
    }

    if (cact_is_symbol(x)) {
        struct cact_val found = cact_env_lookup(cact, cact->current_env, x);
        if (cact_is_null(found)) {
            return cact_make_error(cact, "No such symbol", x);
        }
        return cact_cdr(cact, found);
    }

    if (cact_is_pair(x)) {
        struct cact_val operator = cact_car(cact, x);
        struct cact_val operands = cact_cdr(cact, x);
        struct cact_val maybe_procedure = cact_eval(cact, operator);

        if (cact_is_error(maybe_procedure)) {
            return maybe_procedure;
        }

        if (! cact_is_procedure(maybe_procedure)) {
            fprintf(stdout, "Cannot apply non-operation %s:\n", cact_type_str(maybe_procedure));
            print_sexp(maybe_procedure);
            abort();
        }

        struct cact_proc *proc = cact_to_procedure(maybe_procedure, "eval"); 

        return cact_proc_apply(cact, proc, operands);
    }

    if (cact_is_error(x)) {
	    return x;
    }

	fprintf(stderr, "Cannot evaluate, got type %s.\n", cact_type_str(x));
    abort();
}

/* Evaluate a file using the interpreter. */
struct cact_val
cact_eval_file(struct cactus *cact, FILE *in)
{
    DBG("Running file. \n");
    return cact_eval_string(cact, slurp(in));
}

/* Evaluate a string using the interpreter. */
struct cact_val 
cact_eval_string(struct cactus *cact, char *s)
{
    struct cact_val exp = CACT_NULL_VAL;
    struct cact_val ret = CACT_NULL_VAL;

    cact_lexer_init(&cact->lexer, s);

    while (*s != '\0') {
        int status = 0;
        DBG("Reading new sexp. \n");

        status = cact_read(cact, &exp);
        if (status != CACT_READ_OK) {
            switch (status) {

            case CACT_READ_END_OF_FILE:
                goto STOP_RUNNING;

            case CACT_READ_UNMATCHED_CHAR:
                fprint_sexp(stderr, exp);
                return ret;

            case CACT_READ_OTHER_ERROR:
                fprintf(stderr, "unknown error\n");
                fprint_sexp(stderr, exp);
                break;
            }
        }

        ret = cact_eval(cact, exp);
    }
STOP_RUNNING:
    return ret;
}

struct cact_val
cact_eval_list(struct cactus *cact, struct cact_val lst)
{
    struct cact_val ret;

	ret = CACT_NULL_VAL;

    CACT_LIST_FOR_EACH_ITEM(cact, item, lst) {
	    ret = cact_eval(cact, item);
    }

    return ret;
}

