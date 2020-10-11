
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

static bool
is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x)
{
    if (! cact_is_pair(x))
        return false;

    struct cact_val operator = cact_car(cact, x);
    struct cact_symbol *tagsym = cact_get_symbol(cact, tag);

    if (! cact_is_symbol(operator)) {
        return false;
    }

    struct cact_symbol *opsym = cact_to_symbol(operator, "is_tagged_pair");

    return tagsym == opsym;
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

static bool
is_variable(struct cact_val x)
{
    return cact_is_symbol(x);
}

/*
 * Quotation.
 */

static bool
is_quotation(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "quote", x) || is_tagged_pair(cact, "quasiquote", x);
}

struct cact_val
special_quote(struct cactus *cact, struct cact_val args)
{
    return args;
}

/*
 * Conditionals.
 */

static bool
is_conditional(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "if", x);
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

static bool
is_definition(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "define", x);
}

struct cact_val
special_define(struct cactus *cact, struct cact_val args)
{
    struct cact_val term, defn, value, params;

    term = cact_car(cact, args);

    if (cact_is_symbol(term)) {
        defn = cact_cadr(cact, args);
        value = cact_eval(cact, defn);
    } else if (cact_is_pair(term)) {
        defn = cact_cdr(cact, args);
        params = cact_cdr(cact, term);
        term = cact_car(cact, term);
        value = cact_make_procedure(cact, cact_current_env(cact), params, defn);
    }

    return cact_env_define(cact, cact_current_env(cact), cact_to_symbol(term, "special_define"), value);
}

static bool
is_lambda(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "lambda", x);
}

struct cact_val
special_lambda(struct cactus *cact, struct cact_val args)
{
    struct cact_val lambda_args = cact_car(cact, args);
    struct cact_val body = cact_cdr(cact, args);
    return cact_make_procedure(cact, cact_current_env(cact), lambda_args, body);
}

static bool
is_sequence(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "begin", x);
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

static bool
is_assignment(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "set!", x);
}

struct cact_val
special_set_bang(struct cactus *cact, struct cact_val args)
{
    struct cact_val term = cact_car(cact, args);
    struct cact_val defn = cact_cadr(cact, args);
    struct cact_val value = cact_eval(cact, defn);

    return cact_env_set(cact, cact_current_env(cact), cact_to_symbol(term, "special_set_bang"), value);
}

static bool
is_application(struct cactus *cact, struct cact_val x)
{
    return cact_is_pair(x);
}

/* Evaluate an expression. */
struct cact_val
cact_eval(struct cactus *cact, struct cact_val x)
{
    struct cact_val result = CACT_UNDEF_VAL;

    if (cact_is_obj(x)) {
        cact_preserve(cact, x);
    }

    if (is_self_evaluating(x)) {
        DBG("; cactus eval: self evaluating \n");
        result = x;
        goto FINISHED;
    }

    if (is_quotation(cact, x)) {
        DBG("; cactus eval: quotation \n");
        result = special_quote(cact, cact_cdr(cact, x));
        goto FINISHED;
    }

    if (is_assignment(cact, x)) {
        DBG("; cactus eval: assignment \n");
        result = special_set_bang(cact, cact_cdr(cact, x));
        goto FINISHED;
    }

    if (is_definition(cact, x)) {
        DBG("; cactus eval: definition \n");
        result = special_define(cact, cact_cdr(cact, x));
        goto FINISHED;
    }

    if (is_conditional(cact, x)) {
        DBG("; cactus eval: conditional \n");
        result = special_if(cact, cact_cdr(cact, x));
        goto FINISHED;
    }

    if (is_lambda(cact, x)) {
        DBG("; cactus eval: lambda \n");
        result = special_lambda(cact, cact_cdr(cact, x));
        goto FINISHED;
    }

    if (is_sequence(cact, x)) {
        DBG("; cactus eval: sequence \n");
        result = special_begin(cact, cact_cdr(cact, x));
        goto FINISHED;
    }

    if (is_variable(x)) {
        DBG("; cactus eval: variable lookup \n");
        result = cact_env_lookup(cact, cact_current_env(cact), cact_to_symbol(x, "eval"));
        goto FINISHED;
    }

    if (is_application(cact, x)) {
        DBG("; cactus eval: function application\n");

        struct cact_val operator = cact_car(cact, x);
        struct cact_val operands = cact_cdr(cact, x);
        struct cact_val maybe_procedure = cact_eval(cact, operator);

        if (cact_is_error(maybe_procedure)) {
            return maybe_procedure;
        }

        if (! cact_is_procedure(maybe_procedure)) {
            cact_display(maybe_procedure);
            fprintf(stdout, "Cannot apply non-operation %s:\n", cact_type_str(maybe_procedure));
            abort();
        }

        struct cact_proc *proc = cact_to_procedure(maybe_procedure, "eval");

        result = cact_proc_apply(cact, proc, operands);

        goto FINISHED;
    }

    if (cact_is_error(x)) {
        result = x;
        goto FINISHED;
    }

    fprintf(stderr, "Cannot evaluate, got type %s.\n", cact_type_str(x));
    abort();

FINISHED:

    if (cact_is_obj(x)) {
        cact_unpreserve(cact, x);
    }

    return result;
}

/* Evaluate a file using the interpreter. */
struct cact_val
cact_eval_file(struct cactus *cact, FILE *in)
{
    return cact_eval_string(cact, slurp(in));
}

/* Evaluate a string using the interpreter. */
struct cact_val
cact_eval_string(struct cactus *cact, char *s)
{
    struct cact_val exp = CACT_NULL_VAL;
    struct cact_val ret = CACT_UNDEF_VAL;

    cact_lexer_init(&cact->lexer, s);

    while (*s != '\0') {
        int status = 0;

        status = cact_read(cact, &exp);
        if (status != CACT_READ_OK) {
            switch (status) {

            case CACT_READ_END_OF_FILE:
                goto STOP_RUNNING;

            case CACT_READ_UNMATCHED_CHAR:
                cact_fdisplay(stderr, exp);
                return ret;

            case CACT_READ_OTHER_ERROR:
                fprintf(stderr, "unknown error\n");
                cact_fdisplay(stderr, exp);
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

