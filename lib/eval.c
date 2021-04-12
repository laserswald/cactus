
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

/*
 * High-level evaluation.
 */

struct cact_val cact_eval(struct cactus *cact, struct cact_val expr);

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

// TODO: evaluate last item as a tail call here.
struct cact_val
cact_eval_list(struct cactus *cact, struct cact_val lst)
{
    struct cact_val ret = CACT_NULL_VAL;

    CACT_LIST_FOR_EACH_ITEM(cact, item, lst) {
        ret = cact_eval(cact, item);
    }

    return ret;
}

// Declarations...
void cact_call(struct cactus *, struct cact_proc *, struct cact_val);
void cact_call_stack_pop(struct cactus *cact);
void cact_eval_prim(struct cactus *, struct cact_val, bool);
void cact_tailcall(struct cactus *, struct cact_proc *, struct cact_val);

/* Some constants for what happens when we get back from setjmp */
enum {
	CACT_JMP_INITIAL = 0, /* Set up the call and such */
	CACT_JMP_CALL, /* Tail call a new procedure */
	CACT_JMP_RETURN, /* We are done here */
};

/* 
 * Evaluate a single expression. 
 */
struct cact_val
cact_eval_single(struct cactus *cact, struct cact_val expr)
{	
	cc->evlist = cact_cons(expr, CACT_NULL_VAL);
	cact_eval_start(cact);
}

/*
 * Begin evaluation.
 */
struct cact_val
cact_eval_start(struct cactus *cact)
{
	while (! cact_is_null(cc->evlist)) {
		// Continuations have a jmp_buf to return here after they are done
		// evaluating. This makes recursive function calls efficient spatially.
		switch (setjmp(cc->bounce)) {
	    case CACT_JMP_INITIAL:
		    cact_eval_prim(cact, cact_car(cact, cc->evlist), true);
		    break;

		case CACT_JMP_TCALL:
			cact_proc_apply(cact, cc->proc, cc->args);
			break;

		case CACT_JMP_RETURN:
			cact_call_stack_pop(cact);
			break;

		default:
			cact_fatal("cact_cont_start: got mystery jump return value");
		}

		return cc->retval;
	}
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

struct cact_val special_quote(struct cactus *cact, struct cact_val args);
struct cact_val special_variable(struct cactus *cact, struct cact_val args);
struct cact_val special_set_bang(struct cactus *cact, struct cact_val args);
struct cact_val special_define(struct cactus *cact, struct cact_val args);

struct cact_val special_if(struct cactus *cact, struct cact_val args, bool tail);
struct cact_val special_lambda(struct cactus *cact, struct cact_val args, bool tail);
struct cact_val special_begin(struct cactus *cact, struct cact_val args, bool tail);
struct cact_val special_call(struct cactus *cact, struct cact_val args, bool tail);

/**
 * Evaluate a primitive expression. 
 * 
 * Dispatches to the specific evaluator by looking at it's recognition function.
 * 
 * Calls to this function will not return. Instead, a `longjmp` will be called to 
 * go back to `cact_eval`.
 */
struct cact_val
cact_eval_prim(struct cactus *cact, struct cact_val x, bool tail)
{
	struct cact_cont *cc = cact_current_cont(cact);

    if (cact_is_obj(x)) {
        cact_preserve(cact, x);
    }

    if (is_self_evaluating(x)) {
        DBG("; cactus eval: self evaluating \n");
        cact_return(cact, x);
    }

    if (is_quotation(cact, x)) {
        DBG("; cactus eval: quotation \n");
        cact_return(cact, special_quote(cact, cact_cdr(cact, x)));
    }

    if (is_assignment(cact, x)) {
        DBG("; cactus eval: assignment \n");
        cc->res = special_set_bang(cact, cact_cdr(cact, x));
    }

    if (is_definition(cact, x)) {
        DBG("; cactus eval: definition \n");
        cc->res = special_define(cact, cact_cdr(cact, x));
    }

    if (is_conditional(cact, x)) {
        DBG("; cactus eval: conditional \n");
        cc->res = special_if(cact, cact_cdr(cact, x), tail);
    }

    if (is_lambda(cact, x)) {
        DBG("; cactus eval: lambda \n");
        cc->res = special_lambda(cact, cact_cdr(cact, x));
    }

    if (is_sequence(cact, x)) {
        DBG("; cactus eval: sequence \n");
        cc->res = special_begin(cact, cact_cdr(cact, x));
    }

    if (is_variable(x)) {
        DBG("; cactus eval: variable lookup \n");
        cc->res = cact_env_lookup(cact, cact_current_env(cact), cact_to_symbol(x, "eval"));
    }

    if (is_application(cact, x)) {
        DBG("; cactus eval: function application\n");
        special_call(cact, x, tail);
    }

    fprintf(stderr, "Cannot evaluate, got type %s.\n", cact_type_str(x));
    abort();
}

struct cact_val
special_quote(struct cactus *cact, struct cact_val args)
{
    return args;
}

struct cact_val
special_if(struct cactus *cact, struct cact_val args, bool tail)
{
    struct cact_val cond = cact_car(cact, args);
    struct cact_val cseq = cact_cadr(cact, args);
    struct cact_val alt = cact_caddr(cact, args);
    struct cact_val result = CACT_UNDEF_VAL;

    if (cact_is_truthy(cact_eval_prim(cact, cond, false))) {
        cact_eval_prim(cact, cseq, tail);
    } else {
        cact_eval_prim(cact, alt, tail);
    }
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

struct cact_val
special_lambda(struct cactus *cact, struct cact_val args)
{
    struct cact_val lambda_args = cact_car(cact, args);
    struct cact_val body = cact_cdr(cact, args);
    return cact_make_procedure(cact, cact_current_env(cact), lambda_args, body);
}

struct cact_val
special_begin(struct cactus *cact, struct cact_val args)
{
    if (cact_is_pair(args)) {

	    struct cact_val current_expr_pair = args;
	    while (cact_is_pair(cact_cdr(cact, current_arg_pair))) {
		    struct cact_val current_expr = cact_car(cact, current_expr_pair);
            cact_eval_prim(cact, current_expr, false);
            current_expr_pair = cact_cdr(cact, current_expr_pair);
	    }
	    if (! cact_is_null()) {
		    // TODO: throw an error
	    }
        cact_eval_prim(cact, expr, true);
    } else {
        cact_eval_prim(cact, args, true);
    }
}

struct cact_val
special_set_bang(struct cactus *cact, struct cact_val args)
{
    struct cact_val term = cact_car(cact, args);
    struct cact_val defn = cact_cadr(cact, args);
    struct cact_val value = cact_eval(cact, defn);

    return cact_env_set(cact, cact_current_env(cact), cact_to_symbol(term, "special_set_bang"), value);
}

void
special_call(struct cactus *cact, struct cact_val x, bool tail)
{
    struct cact_val operator = cact_car(cact, x);
    struct cact_val operands = cact_cdr(cact, x);

    // Make sure the operator is OK.
    struct cact_val maybe_procedure = cact_eval_prim(cact, operator);

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

/* Jump back to the current trampoline, but without adding more stuff to do. */
void
cact_return(struct cactus *cact, struct cact_val value)
{
	struct cact_cont *current_cont = cact_current_continuation(cact);

	cont->retval = value;

	longjmp(cont->bounce, CACT_JMP_RETURN);
}

/* Jump back to the trampoline, but replace the return value of that trampoline with 
 * the return value of */
void
cact_tailcall(struct cactus *cact, struct cact_proc *proc, struct cact_val args)
{
	struct cact_cont *current_cont = cact_current_continuation(cact);

	cont->proc = proc;
	cont->evlist = CACT_NULL_VAL;
	cont->argl = args;

	longjmp(cont->bounce, CACT_JMP_TCALL);
}

void
cact_call(struct cactus *cact, struct cact_proc *proc, struct cact_val args)
{
    struct cact_cont *nc;

    assert(cact);
    assert(frame);

    nc = (struct cact_cont *) cact_alloc(cact, CACT_OBJ_CONT);
    cact_cont_init(nc, proc);

    SLIST_INSERT_HEAD(&cact->conts, nc, parent);

	cact_tailcall(cact, proc, args);
}

/* Finish a function call or macro expansion. */
void
cact_call_stack_pop(struct cactus *cact)
{
    assert(cact);

    SLIST_REMOVE_HEAD(&cact->conts, parent);
}

/*
 * Syntax recognizers.
 */

bool is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x);

static bool
is_self_evaluating(struct cact_val x)
{
    return cact_is_null(x)
           || cact_is_bool(x)
           || cact_is_number(x)
           || cact_is_procedure(x)
           || cact_is_string(x)
           || cact_is_error(x);
           || cact_is_env(x);
}

static bool
is_variable(struct cact_val x)
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

