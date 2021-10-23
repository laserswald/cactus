
#include "cactus/eval.h"

#include "cactus/core.h"
#include "cactus/write.h"
#include "cactus/cont.h"

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

/*
 * High-level evaluation.
 */

struct cact_val cact_eval_single(struct cactus *cact, struct cact_val expr);

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
        ret = cact_eval_single(cact, exp);
    }
STOP_RUNNING:
    return ret;
}

// TODO: evaluate last item as a tail call here.
struct cact_val
cact_eval_list(struct cactus *cact, struct cact_val lst)
{
    struct cact_val ret = CACT_NULL_VAL;

    CACT_LIST_FOR_EACH_ITEM(cact, item, lst) {
        ret = cact_eval_single(cact, item);
    }

    return ret;
}

// Declarations...
void cact_call(struct cactus *, struct cact_proc *, struct cact_val);
void cact_call_stack_pop(struct cactus *cact);
void cact_eval_prim(struct cactus *, struct cact_val, bool);
void cact_tailcall(struct cactus *, struct cact_proc *, struct cact_val);

/*
 * Begin evaluation.
 */
struct cact_val
cact_eval_start(struct cactus *cact)
{
	struct cact_cont *cc = cact_current_cont(cact);
	return cact_resume_cont(cact, cc);
}

/*
 * Evaluate a single expression.
 */
struct cact_val
cact_eval_single(struct cactus *cact, struct cact_val expr)
{
	struct cact_cont *cc = cact_current_cont(cact);
    cc->evlist = cact_cons(cact, expr, CACT_NULL_VAL);
    return cact_eval_start(cact);
}


/*
 * Primitive evaluation recognizers and helpers.
 */

static bool is_self_evaluating(struct cact_val x);
static bool is_variable(struct cactus *cact, struct cact_val x);
static bool is_quotation(struct cactus *cact, struct cact_val x);
static bool is_assignment(struct cactus *cact, struct cact_val x);
static bool is_definition(struct cactus *cact, struct cact_val x);
static bool is_conditional(struct cactus *cact, struct cact_val x);
static bool is_lambda(struct cactus *cact, struct cact_val x);
static bool is_sequence(struct cactus *cact, struct cact_val x);
static bool is_application(struct cactus *cact, struct cact_val x);

void special_quote(struct cactus *cact, struct cact_val args);
void special_variable(struct cactus *cact, struct cact_val args);
void special_set_bang(struct cactus *cact, struct cact_val args);
void special_define(struct cactus *cact, struct cact_val args);

// Tail call sensitive special forms.
void special_if(struct cactus *cact, struct cact_val args, bool tail);
void special_lambda(struct cactus *cact, struct cact_val args, bool tail);
void special_begin(struct cactus *cact, struct cact_val args, bool tail);
void special_call(struct cactus *cact, struct cact_val args, bool tail);

/**
 * Evaluate a primitive expression.
 *
 * Dispatches to the specific evaluator by looking at it's recognition function.
 *
 * Calls to this function will not return. Instead, a `longjmp` will be called to
 * go back to `cact_eval`.
 */
void
cact_eval_prim(struct cactus *cact, struct cact_val x, bool tail)
{
    if (cact_is_obj(x)) {
        cact_preserve(cact, x);
    }

    if (is_self_evaluating(x)) {
        DBG("; cactus eval: self evaluating \n");
        cact_return(cact, x);
    }

    if (is_quotation(cact, x)) {
        DBG("; cactus eval: quotation \n");
        special_quote(cact, cact_cdr(cact, x));
    }

    if (is_assignment(cact, x)) {
        DBG("; cactus eval: assignment \n");
        special_set_bang(cact, cact_cdr(cact, x));
    }

    if (is_definition(cact, x)) {
        DBG("; cactus eval: definition \n");
        special_define(cact, cact_cdr(cact, x));
    }

    if (is_conditional(cact, x)) {
        DBG("; cactus eval: conditional \n");
        special_if(cact, cact_cdr(cact, x), tail);
    }

    if (is_lambda(cact, x)) {
        DBG("; cactus eval: lambda \n");
        special_lambda(cact, cact_cdr(cact, x), tail);
    }

    if (is_sequence(cact, x)) {
        DBG("; cactus eval: sequence \n");
        special_begin(cact, cact_cdr(cact, x), tail);
    }

    if (is_variable(cact, x)) {
        DBG("; cactus eval: variable lookup \n");
        cact_env_lookup(cact, cact_current_env(cact), cact_to_symbol(x, "eval"));
    }

    if (is_application(cact, x)) {
        DBG("; cactus eval: function application\n");
        special_call(cact, x, tail);
    }

    fprintf(stderr, "Cannot evaluate, got type %s.\n", cact_type_str(x));
    abort();
}

void
special_quote(struct cactus *cact, struct cact_val args)
{
    cact_return(cact, args);
}

void
special_if(struct cactus *cact, struct cact_val args, bool tail)
{
    struct cact_val cond = cact_car(cact, args);
    struct cact_val cseq = cact_cadr(cact, args);
    struct cact_val alt = cact_caddr(cact, args);

	cact_eval_prim(cact, cond, false);

    if (cact_is_truthy(cact_current_retval(cact))) {
        cact_eval_prim(cact, cseq, tail);
    } else {
        cact_eval_prim(cact, alt, tail);
    }
}

void
special_define(struct cactus *cact, struct cact_val args)
{
    struct cact_val term, defn, value, params;

    term = cact_car(cact, args);

    if (cact_is_symbol(term)) {
        defn = cact_cadr(cact, args);
        value = cact_eval_single(cact, defn);
    } else if (cact_is_pair(term)) {
        defn = cact_cdr(cact, args);
        params = cact_cdr(cact, term);
        term = cact_car(cact, term);
        value = cact_make_procedure(cact, cact_current_env(cact), params, defn);
    }

    cact_return(cact, cact_env_define(cact, cact_current_env(cact), cact_to_symbol(term, "special_define"), value));
}

void
special_lambda(struct cactus *cact, struct cact_val args, bool tail)
{
    struct cact_val lambda_args = cact_car(cact, args);
    struct cact_val body = cact_cdr(cact, args);
    cact_return(cact, cact_make_procedure(cact, cact_current_env(cact), lambda_args, body));
}

void
special_begin(struct cactus *cact, struct cact_val args, bool tail)
{
    if (cact_is_pair(args)) {
        struct cact_val current_expr_pair = args;
        while (cact_is_pair(cact_cdr(cact, current_expr_pair))) {
            struct cact_val current_expr = cact_car(cact, current_expr_pair);
            cact_eval_prim(cact, current_expr, false);
            current_expr_pair = cact_cdr(cact, current_expr_pair);
        }
        cact_eval_prim(cact, cact_car(cact, current_expr_pair), true);
    } else {
        cact_eval_prim(cact, args, true);
    }
}

void
special_set_bang(struct cactus *cact, struct cact_val args)
{
    struct cact_val term = cact_car(cact, args);
    struct cact_val defn = cact_cadr(cact, args);

	cact_eval_prim(cact, defn, true);    
	struct cact_val value = cact_current_retval(cact);

    cact_env_set(cact, cact_current_env(cact), cact_to_symbol(term, "special_set_bang"), value);
}

void
special_call(struct cactus *cact, struct cact_val x, bool tail)
{
    struct cact_val operator = cact_car(cact, x);
    struct cact_val operands = cact_cdr(cact, x);

    // Make sure the operator is OK.
    cact_eval_prim(cact, operator, false);
    struct cact_val maybe_procedure = cact_current_retval(cact);

    if (cact_is_error(maybe_procedure)) {
        cact_return(cact, maybe_procedure);
    }
    if (! cact_is_procedure(maybe_procedure)) {
        cact_display(maybe_procedure);
        fprintf(stdout, "Cannot apply non-operation %s:\n", cact_type_str(maybe_procedure));
        abort();
    }

    struct cact_proc *proc = cact_to_procedure(maybe_procedure, "eval");

    if (tail) {
        cact_tailcall(cact, proc, operands);
    } else {
        cact_call(cact, proc, operands);
    }
}

/*
 * Helpers for the primitive operation definitions above.
 */

/*
 * Syntax recognizers.
 */

static bool is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x);

static bool
is_self_evaluating(struct cact_val x)
{
    return cact_is_null(x)
           || cact_is_bool(x)
           || cact_is_number(x)
           || cact_is_procedure(x)
           || cact_is_string(x)
           || cact_is_error(x)
		   || cact_is_env(x);
}

static bool
is_variable(struct cactus *cact, struct cact_val x)
{
    return cact_is_symbol(x);
}

static bool
is_quotation(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "quote", x) || is_tagged_pair(cact, "quasiquote", x);
}

static bool
is_conditional(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "if", x);
}

static bool
is_definition(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "define", x);
}

static bool
is_lambda(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "lambda", x);
}

static bool
is_sequence(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "begin", x);
}

static bool
is_assignment(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "set!", x);
}

static bool
is_application(struct cactus *cact, struct cact_val x)
{
    return cact_is_pair(x);
}

static bool
is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return false;
    }

    struct cact_val operator = cact_car(cact, x);
    struct cact_symbol *tagsym = cact_get_symbol(cact, tag);

    if (! cact_is_symbol(operator)) {
        return false;
    }

    struct cact_symbol *opsym = cact_to_symbol(operator, "is_tagged_pair");

    return tagsym == opsym;
}

