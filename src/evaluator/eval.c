// The evaluation module.

#include "evaluator/eval.h"

#include "core.h"
#include "write.h"
#include "evaluator/cont.h"

#include "val.h"
#include "sym.h"
#include "str.h"
#include "env.h"
#include "proc.h"
#include "bool.h"
#include "num.h"
#include "err.h"
#include "syn.h"

#include "internal/debug.h"
#include "internal/utils.h"

/*
 * High-level evaluation.
 */

struct cact_val cact_eval_single(struct cactus *cact, struct cact_val expr);

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

/*
 * Evaluate a single expression.
 */
struct cact_val
cact_eval_single(struct cactus *cact, struct cact_val expr)
{
    struct cact_cont *cc = cact_current_cont(cact);
    cc->expr = expr;
    cact_cont_start(cact, cc);
    return cc->retval;
}

/*
 * Primitive evaluation recognizers and helpers.
 */

void special_quote(struct cactus *cact, struct cact_val args);
void special_variable(struct cactus *cact, struct cact_val args);
void special_set_bang(struct cactus *cact, struct cact_val args);
void special_define(struct cactus *cact, struct cact_val args);
void special_if(struct cactus *cact, struct cact_val args);
void special_lambda(struct cactus *cact, struct cact_val args);
void special_begin(struct cactus *cact, struct cact_val args);
void special_call(struct cactus *cact, struct cact_val args);
void cact_eval_start_include(struct cactus *cact, struct cact_val expr);

/**
 * Evaluate a primitive expression.
 *
 * Dispatches to the specific evaluator by looking at it's recognition function.
 *
 * Calls to this function will not return. Instead, a `longjmp` will be called to
 * go back to `cact_eval`.
 */
void
cact_eval_prim(struct cactus *cact, struct cact_cont *cc)
{
    if (is_self_evaluating(cc->expr)) {
        cact_cont_return(cc, cc->expr);
    }

    if (is_quotation(cact, cc->expr)) {
        special_quote(cact, cact_cdr(cact, cc->expr));
    }

    if (is_assignment(cact, cc->expr)) {
        special_set_bang(cact, cact_cdr(cact, cc->expr));
    }

    if (is_definition(cact, cc->expr)) {
        special_define(cact, cact_cdr(cact, cc->expr));
    }

    if (is_conditional(cact, cc->expr)) {
        special_if(cact, cact_cdr(cact, cc->expr));
    }

    if (is_lambda(cact, cc->expr)) {
        special_lambda(cact, cact_cdr(cact, cc->expr));
    }

    if (is_sequence(cact, cc->expr)) {
        special_begin(cact, cact_cdr(cact, cc->expr));
    }

    if (is_variable(cact, cc->expr)) {
        assert(cact_env_num_bindings(cc->env) != 0);
        cact_cont_return(
            cc,
            cact_env_lookup(
                cact,
                cc->env,
                cact_to_symbol(cc->expr, "eval")
            )
        );
    }

    if (is_inclusion(cact, cc->expr)) {
        cact_eval_start_include(cact, cc->expr);
    }

    if (is_application(cact, cc->expr)) {
        special_call(cact, cc->expr);
    }

    fprintf(stderr, "Cannot evaluate, got type %s.\n", cact_type_str(cc->expr));
    abort();
}

struct cact_cont*
push_new_cont(struct cactus *cact, struct cact_cont *cc)
{
    struct cact_cont *nc = (struct cact_cont *) cact_alloc(cact, CACT_OBJ_CONT);
    cact_cont_init(nc, cc->env, NULL);
    cact_call_stack_push(cact, nc);
    return nc;
}

/*
 * Evaluate a quotation.
 */
void
special_quote(struct cactus *cact, struct cact_val args)
{
    struct cact_cont *cc = cact_current_cont(cact);
    cact_cont_return(cc, args);
}

/**
 * Evaluate a definition.
 *
 * Here we unpack the definition and queue the term as the unevaled
 * item and the next expression to evaluate as the
 */
void
special_define(struct cactus *cact, struct cact_val args)
{
    struct cact_cont *cc = cact_current_cont(cact);
    struct cact_val term, definition;

    term = cact_car(cact, args);
    definition = cact_cadr(cact, args);

    cc->unevaled = term;
    cc->state = CACT_JMP_DEFINE;

    struct cact_cont *nc = push_new_cont(cact, cc);
    nc->expr = definition;
    cact_cont_start(cact, nc);
}

void
special_lambda(struct cactus *cact, struct cact_val args)
{
    struct cact_cont *cc = cact_current_cont(cact);
    struct cact_val lambda_args = cact_car(cact, args);
    struct cact_val body = cact_cdr(cact, args);

    assert(cact_env_num_bindings(cc->env) != 0);

    cact_cont_return(cc, cact_make_procedure(cact, cc->env, lambda_args, body));
}

void
special_begin(struct cactus *cact, struct cact_val args)
{
    struct cact_cont *cc = cact_current_cont(cact);

    cc->retval = CACT_UNDEF_VAL;
    cc->unevaled = args;
    cact_cont_step(cc, CACT_JMP_EVAL_SEQ);
}

void
special_set_bang(struct cactus *cact, struct cact_val args)
{
    struct cact_cont *cc = cact_current_cont(cact);
    assert(cact_env_num_bindings(cc->env) != 0);
    struct cact_val term, defn;

    term = cact_car(cact, args);
    defn = cact_cadr(cact, args);

    cc->state = CACT_JMP_ASSIGN;
    cc->unevaled = term;

    struct cact_cont *nc = push_new_cont(cact, cc);
    nc->expr = defn;

    cact_cont_start(cact, nc);
}

/**
 * Unpack the if statement and set up context for the conditional expression.
 *
 * Should queue the if's conditional to be evaluated, and the branches
 * to be unevaluated.
 */
void
special_if(struct cactus *cact, struct cact_val args)
{
    struct cact_cont *cc = cact_current_cont(cact);
    assert(cact_env_num_bindings(cc->env) != 0);

    struct cact_val condition = cact_car(cact, args);

    cc->unevaled = cact_cdr(cact, args);
    cc->state = CACT_JMP_IF_DECISION;

    struct cact_cont *nc = push_new_cont(cact, cc);
    nc->expr = condition;
    nc->state = CACT_JMP_EVAL_SINGLE;

    cact_cont_start(cact, nc);
}

/**
 * Look at the truthiness of the current return value, and perform one of the
 * unevaluated expressions depending on that.
 */
void
cact_eval_branch(struct cactus *cact, struct cact_cont *cc)
{
    assert(cact_env_num_bindings(cc->env) != 0);
    if (cact_is_truthy(cc->retval)) {
        cc->retval = CACT_UNDEF_VAL;
        cc->expr = cact_car(cact, cc->unevaled);
        cc->state = CACT_JMP_EVAL_SINGLE;
        cact_cont_step(cc, CACT_JMP_EVAL_SINGLE);
    }

    if (cact_is_null(cact_cdr(cact, cc->unevaled))) {
        cact_cont_return(cc, CACT_UNDEF_VAL);
    }

    cc->expr = cact_cadr(cact, cc->unevaled);
    cact_cont_step(cc, CACT_JMP_EVAL_SINGLE);
}

/*
 * Finish evaluating a definition.
 */
void
cact_eval_definition(struct cactus *cact, struct cact_cont *cc)
{
    struct cact_val term, defn;

    term = cc->unevaled;
    defn = cc->retval;

    assert(cact_env_num_bindings(cc->env) != 0);
    cact_env_define(cact, cc->env, cact_to_symbol(term, "special_define"), defn);
    assert(cact_env_num_bindings(cc->env) != 0);

    cact_cont_finish(cc);
}


/**
 * Evaluate a sequence of expressions.
 *
 * This will be entered after the evaluator has evaluated a single expression
 * and there are more expressions to be evaluated with the `begin` semantics;
 * that is, each expression is evaluated and all return values except the last
 * are discarded.
 */
void
cact_eval_sequence(struct cactus *cact, struct cact_cont *cc)
{
    // If we have no more items to evaluate, then we are done.
    if (cact_is_null(cc->unevaled)) {
        cact_cont_finish(cc);
    }

    if (cact_is_null(cact_cdr(cact, cc->unevaled))) {
        cc->expr = cact_car(cact, cc->unevaled);
        cact_cont_step(cc, CACT_JMP_EVAL_SINGLE);
    }

    struct cact_cont *nc = push_new_cont(cact, cc);
    nc->expr = cact_car(cact, cc->unevaled);
    cc->unevaled = cact_cdr(cact, cc->unevaled);
    cact_cont_start(cact, nc);
}

void
cact_eval_assignment(struct cactus *cact, struct cact_cont *cc)
{
    struct cact_val term, defn;

    term = cc->unevaled;
    defn = cc->retval;

    assert(cc->env->entries.count != 0);
    cact_env_set(cact, cc->env, cact_to_symbol(term, "special_set_bang"), defn);
    cact_cont_return(cc, CACT_UNDEF_VAL);
}

/*
 * Procedure application.
 */

void
special_call(struct cactus *cact, struct cact_val x)
{
    struct cact_cont *cc = cact_current_cont(cact);
    cc->unevaled = cact_cdr(cact, x);
    cc->state = CACT_JMP_APPLY_DID_OP;

    struct cact_cont *nc = push_new_cont(cact, cc);
    nc->expr = cact_car(cact, x);
    nc->state = CACT_JMP_EVAL_SINGLE;

    cact_cont_start(cact, nc);
}

/**
 * Extract and evaluate the operator in this procedure application.
 */
void
cact_eval_apply_with_operator(struct cactus *cact, struct cact_cont *cc)
{
    cc->argl = CACT_NULL_VAL;

    struct cact_val maybe_procedure = cc->retval;
    if (! cact_is_procedure(maybe_procedure)) {
        cact_display(maybe_procedure);
        fprintf(stdout, "Cannot apply non-operation %s:\n", cact_type_str(maybe_procedure));
        abort();
    }
    cc->proc = cact_to_procedure(maybe_procedure, "eval");

    if (cact_is_null(cc->unevaled)) {
        cact_cont_step(cc, CACT_JMP_APPLY);
    } else {
        cact_cont_step(cc, CACT_JMP_ARG_POP);
    }
}

/**
 * Loop over the argument list and evaluate each part.
 */
void
cact_eval_arg_pop(struct cactus *cact, struct cact_cont *cc)
{
    if (cact_is_null(cc->unevaled)) {
        if (cc->proc->nativefn) {
            cact_cont_step(cc, CACT_JMP_APPLY);
        }
        cact_cont_step(cc, CACT_JMP_EXTEND_ENV);
    }

    struct cact_cont *nc = push_new_cont(cact, cc);
    nc->expr = cact_car(cact, cc->unevaled);
    nc->state = CACT_JMP_EVAL_SINGLE;

    cc->unevaled = cact_cdr(cact, cc->unevaled);
    cc->state = CACT_JMP_BIND_ARG;

    cact_cont_start(cact, nc);
}

void
cact_eval_arg_bind(struct cactus *cact, struct cact_cont *cc)
{
    cc->argl = cact_append(cact, cc->argl, cc->retval);
    cact_cont_step(cc, CACT_JMP_ARG_POP);
}

void
cact_eval_extend_env(struct cactus *cact, struct cact_cont *cc)
{
    struct cact_env *extended = (struct cact_env *) cact_alloc(cact, CACT_OBJ_ENVIRONMENT);
    cact_env_init(extended, cc->proc->env);
    assert(cact_env_num_bindings(extended) != 0);

    cact_preserve(cact, CACT_OBJ_VAL(extended));

    // Iterate over the binding names (the argl of proc)
    CACT_LIST_FOR_EACH_ITEM(cact, current_binding_name, cc->proc->argl) {
        if (cact_is_null(cc->argl)) {
            die("eval_args: not enough arguments");
        }

        cact_env_define(
            cact,
            extended,
            cact_to_symbol(current_binding_name, "cact_eval_extend_env"),
            cact_car(cact, cc->argl)
        );

        cc->argl = cact_cdr(cact, cc->argl);
    }
    cc->env = extended;

    cact_unpreserve(cact, CACT_OBJ_VAL(extended));
    assert(cact_env_num_bindings(cc->env) != 0);

    cact_cont_step(cc, CACT_JMP_APPLY);
}

void
cact_eval_apply(struct cactus *cact, struct cact_cont *cc)
{
    struct cact_proc *clo = cc->proc;

    if (clo->nativefn != NULL) {
        cact_cont_return(cc, clo->nativefn(cact, cc->argl));
    } else {
        cc->unevaled = cc->proc->body;
        cact_cont_step(cc, CACT_JMP_EVAL_SEQ);
    }
}

void
cact_eval_start_include(struct cactus *cact, struct cact_val expr)
{
    struct cact_cont *cc = cact_current_cont(cact);
    struct cact_val fname = cact_cadr(cact, expr);
    if (! cact_is_string(fname)) {
        cact_raise(cact, cact_make_error(cact, "`include` expects a string", expr));
    }

    FILE *f = fopen(cact_to_string(fname, "include")->str, "r");
    if (! f) {
        cact_raise(cact, cact_make_error(cact, "include: no file found", fname));
    }

    cact_lexer_init(&cc->lexer, slurp(f));
    cact_cont_step(cc, CACT_JMP_LOAD);
}

void
cact_eval_include(struct cactus *cact, struct cact_cont *cc)
{
    struct cact_val v;

    int status = cact_read(cact, &cc->lexer, &v);
    cact_preserve(cact, v);

    if (status == CACT_READ_OK) {
        struct cact_cont *nc = push_new_cont(cact, cc);
        nc->expr = v;
        cact_cont_start(cact, nc);
    }

    cact_unpreserve(cact, v);

    switch (status) {
    case CACT_READ_END_OF_FILE:
        // We are done, let's finish up
        cact_cont_finish(cc);
        break;

    case CACT_READ_UNMATCHED_CHAR:
        cact_raise(cact, v);

    case CACT_READ_OTHER_ERROR:
        cact_raise(cact, v);
    }

}

