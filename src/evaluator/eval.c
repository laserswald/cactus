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

cact_value_t cact_eval_single(cact_context_t *cact, cact_value_t expr);

/* Evaluate a file using the interpreter. */
cact_value_t
cact_eval_file(cact_context_t *cact, FILE *in)
{
    return cact_eval_string(cact, slurp(in));
}

/* Evaluate a string using the interpreter. */
cact_value_t
cact_eval_string(cact_context_t *cact, char *s)
{
    cact_value_t exp = CACT_NULL_VAL;
    cact_value_t ret = CACT_UNDEF_VAL;

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
cact_value_t
cact_eval_list(cact_context_t *cact, cact_value_t lst)
{
    cact_value_t ret = CACT_NULL_VAL;

    CACT_LIST_FOR_EACH_ITEM(cact, item, lst) {
        ret = cact_eval_single(cact, item);
    }

    return ret;
}

/*
 * Evaluate a single expression.
 */
cact_value_t
cact_eval_single(cact_context_t *cact, cact_value_t expr)
{
    cact_frame_t *cc = cact_current_frame(cact);
    cc->expr = expr;
    cact_resume_frame(cact, cc);
    return cc->retval;
}

/*
 * Primitive evaluation recognizers and helpers.
 */

void special_quote(cact_context_t *cact, cact_value_t args);
void special_variable(cact_context_t *cact, cact_value_t args);
void special_set_bang(cact_context_t *cact, cact_value_t args);
void special_define(cact_context_t *cact, cact_value_t args);
void special_if(cact_context_t *cact, cact_value_t args);
void special_lambda(cact_context_t *cact, cact_value_t args);
void special_begin(cact_context_t *cact, cact_value_t args);
void special_call(cact_context_t *cact, cact_value_t args);

/**
 * Evaluate a primitive expression.
 *
 * Dispatches to the specific evaluator by looking at it's recognition function.
 *
 * Calls to this function will not return. Instead, a `longjmp` will be called to
 * go back to `cact_eval`.
 */
void
cact_eval_prim(cact_context_t *cact, cact_frame_t *cc)
{
    if (is_self_evaluating(cc->expr)) {
        cact_leave_frame(cc, cc->expr);
    }

    if (is_quotation(cact, cc->expr)) {
        special_quote(cact, cact_cdr(cact, cc->expr));
    }

    if (is_assignment(cact, cc->expr)) {
        DBG("assignment \n");
        special_set_bang(cact, cact_cdr(cact, cc->expr));
    }

    if (is_definition(cact, cc->expr)) {
        DBG("definition \n");
        special_define(cact, cact_cdr(cact, cc->expr));
    }

    if (is_conditional(cact, cc->expr)) {
        DBG("conditional \n");
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
        cact_leave_frame(
            cc,
            cact_env_lookup(
                cact,
                cc->env,
                cact_to_symbol(cc->expr, "eval")
            )
        );
    }

    if (is_application(cact, cc->expr)) {
        DBG("; cactus eval: function application\n");
        special_call(cact, cc->expr);
    }

    fprintf(stderr, "Cannot evaluate, got type %s.\n", cact_type_str(cc->expr));
    abort();
}

cact_frame_t*
push_new_frame(cact_context_t *cact, cact_frame_t *cc)
{
    cact_frame_t *nc = (cact_frame_t *) cact_alloc(cact, CACT_OBJ_CONT);
    cact_init_frame(nc, cc->env, NULL);
    cact_continuation_push(cact, nc);
    return nc;
}

/*
 * Evaluate a quotation.
 */
void
special_quote(cact_context_t *cact, cact_value_t args)
{
    cact_frame_t *cc = cact_current_frame(cact);
    cact_leave_frame(cc, args);
}

/**
 * Evaluate a definition.
 *
 * Here we unpack the definition and queue the term as the unevaled
 * item and the next expression to evaluate as the
 */
void
special_define(cact_context_t *cact, cact_value_t args)
{
    cact_frame_t *cc = cact_current_frame(cact);
    cact_value_t term, definition;

    term = cact_car(cact, args);
    definition = cact_cadr(cact, args);

    cc->unevaled = term;
    cc->state = CACT_JMP_DEFINE;

    cact_frame_t *nc = push_new_frame(cact, cc);
    nc->expr = definition;

    cact_resume_frame(cact, nc);
}

void
special_lambda(cact_context_t *cact, cact_value_t args)
{
    cact_frame_t *cc = cact_current_frame(cact);
    cact_value_t lambda_args = cact_car(cact, args);
    cact_value_t body = cact_cdr(cact, args);

    assert(cact_env_num_bindings(cc->env) != 0);
    cc->retval = cact_make_procedure(cact, cc->env, lambda_args, body);

    cc->state = CACT_JMP_FINISH;
    cact_continue_frame(cc);
}

void
special_begin(cact_context_t *cact, cact_value_t args)
{
    cact_frame_t *cc = cact_current_frame(cact);

    cc->retval = CACT_UNDEF_VAL;
    cc->unevaled = args;
    cact_continue_frame_step(cc, CACT_JMP_EVAL_SEQ);
}

void
special_set_bang(cact_context_t *cact, cact_value_t args)
{
    cact_frame_t *cc = cact_current_frame(cact);
    assert(cact_env_num_bindings(cc->env) != 0);
    cact_value_t term, defn;

    term = cact_car(cact, args);
    defn = cact_cadr(cact, args);

    cc->state = CACT_JMP_ASSIGN;
    cc->unevaled = term;

    cact_frame_t *nc = push_new_frame(cact, cc);
    nc->expr = defn;

    cact_resume_frame(cact, nc);
}

/**
 * Unpack the if statement and set up context for the conditional expression.
 *
 * Should queue the if's conditional to be evaluated, and the branches
 * to be unevaluated.
 */
void
special_if(cact_context_t *cact, cact_value_t args)
{
    cact_frame_t *cc = cact_current_frame(cact);
    assert(cact_env_num_bindings(cc->env) != 0);
    DBG("Evaluating if statement #%lu\n", cc->obj.store_data.place);

    cact_value_t condition = cact_car(cact, args);

    cc->unevaled = cact_cdr(cact, args);
    cc->state = CACT_JMP_IF_DECISION;

    cact_frame_t *nc = push_new_frame(cact, cc);
    nc->expr = condition;
    nc->state = CACT_JMP_EVAL_SINGLE;

    cact_resume_frame(cact, nc);
}

/**
 * Look at the truthiness of the current return value, and perform one of the
 * unevaluated expressions depending on that.
 */
void
cact_eval_branch(cact_context_t *cact, cact_frame_t *cc)
{
    DBG("Evaluating branch #%lu\n", cc->obj.store_data.place);
    assert(cact_env_num_bindings(cc->env) != 0);
    if (cact_is_truthy(cc->retval)) {
        cc->retval = CACT_UNDEF_VAL;
        cc->expr = cact_car(cact, cc->unevaled);
        cc->state = CACT_JMP_EVAL_SINGLE;
        cact_show_continuation(cact);
        cact_continue_frame_step(cc, CACT_JMP_EVAL_SINGLE);
    }

    if (cact_is_null(cact_cdr(cact, cc->unevaled))) {
        cact_leave_frame(cc, CACT_UNDEF_VAL);
    }

    cc->expr = cact_cadr(cact, cc->unevaled);
    cc->state = CACT_JMP_EVAL_SINGLE;
    cact_show_continuation(cact);
    cact_continue_frame(cc);
}

/*
 * Finish evaluating a definition.
 */
void
cact_eval_definition(cact_context_t *cact, cact_frame_t *cc)
{
    cact_value_t term, defn;

    term = cc->unevaled;
    defn = cc->retval;

    assert(cact_env_num_bindings(cc->env) != 0);
    cact_env_define(cact, cc->env, cact_to_symbol(term, "special_define"), defn);
    assert(cact_env_num_bindings(cc->env) != 0);

    cc->state = CACT_JMP_FINISH;
    cact_continue_frame(cc);
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
cact_eval_sequence(cact_context_t *cact, cact_frame_t *cc)
{
    assert(cc->env->entries.count != 0);
    // If we have no more items to evaluate, then we are done.
    if (cact_is_null(cc->unevaled)) {
        cact_continue_frame_step(cc, CACT_JMP_FINISH);
    }

    if (! cact_is_pair(cc->unevaled)) {
        // If we don't have a pair, then queue up this expression for evaluation, and
        // then return it after it's done evaluating.
        cc->expr = cc->unevaled;
        cact_continue_frame_step(cc, CACT_JMP_EVAL_SINGLE);
    }

    cact_frame_t *nc = push_new_frame(cact, cc);
    nc->expr = cact_car(cact, cc->unevaled);
    cc->unevaled = cact_cdr(cact, cc->unevaled);
    cact_resume_frame(cact, nc);
}

void
cact_eval_assignment(cact_context_t *cact, cact_frame_t *cc)
{
    cact_value_t term, defn;

    term = cc->unevaled;
    defn = cc->retval;

    assert(cc->env->entries.count != 0);
    cact_env_set(cact, cc->env, cact_to_symbol(term, "special_set_bang"), defn);

    cc->retval = CACT_UNDEF_VAL;

    cc->state = CACT_JMP_FINISH;
    cact_continue_frame(cc);
}

/*
 * Procedure application.
 */

void
special_call(cact_context_t *cact, cact_value_t x)
{
    cact_frame_t *cc = cact_current_frame(cact);
    cc->unevaled = cact_cdr(cact, x);
    cc->state = CACT_JMP_APPLY_DID_OP;

    cact_frame_t *nc = push_new_frame(cact, cc);
    nc->expr = cact_car(cact, x);
    nc->state = CACT_JMP_EVAL_SINGLE;

    cact_resume_frame(cact, nc);
}

/**
 * Extract and evaluate the operator in this procedure application.
 */
void
cact_eval_apply_with_operator(cact_context_t *cact, cact_frame_t *cc)
{
    cc->argl = CACT_NULL_VAL;

    cact_value_t maybe_procedure = cc->retval;
    if (! cact_is_procedure(maybe_procedure)) {
        cact_display(maybe_procedure);
        fprintf(stdout, "Cannot apply non-operation %s:\n", cact_type_str(maybe_procedure));
        abort();
    }
    cc->proc = cact_to_procedure(maybe_procedure, "eval");

    if (cact_is_null(cc->unevaled)) {
        cc->state = CACT_JMP_APPLY;
        cact_continue_frame(cc);
    } else {
        cc->state = CACT_JMP_ARG_POP;
        cact_continue_frame(cc);
    }
}

/**
 * Loop over the argument list and evaluate each part.
 */
void
cact_eval_arg_pop(cact_context_t *cact, cact_frame_t *cc)
{
    if (cact_is_null(cc->unevaled)) {
        if (cc->proc->nativefn) {
            cact_continue_frame_step(cc, CACT_JMP_APPLY);
        }
        cact_continue_frame_step(cc, CACT_JMP_EXTEND_ENV);
    }

    cact_frame_t *nc = push_new_frame(cact, cc);
    nc->expr = cact_car(cact, cc->unevaled);
    nc->state = CACT_JMP_EVAL_SINGLE;

    cc->unevaled = cact_cdr(cact, cc->unevaled);
    cc->state = CACT_JMP_BIND_ARG;

    cact_resume_frame(cact, nc);
}

void
cact_eval_arg_bind(cact_context_t *cact, cact_frame_t *cc)
{
    cc->argl = cact_append(cact, cc->argl, cc->retval);
    cact_continue_frame_step(cc, CACT_JMP_ARG_POP);
}

void
cact_eval_extend_env(cact_context_t *cact, cact_frame_t *cc)
{
    cact_env_t *extended = (cact_env_t *) cact_alloc(cact, CACT_OBJ_ENVIRONMENT);
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
    print_env(extended);
    cc->env = extended;

    cact_unpreserve(cact, CACT_OBJ_VAL(extended));
    assert(cact_env_num_bindings(cc->env) != 0);

    cact_continue_frame_step(cc, CACT_JMP_APPLY);
}

void
cact_eval_apply(cact_context_t *cact, cact_frame_t *cc)
{
    cact_procedure_t *clo = cc->proc;

    if (clo->nativefn != NULL) {
        cact_leave_frame(cc, clo->nativefn(cact, cc->argl));
    } else {
        cc->unevaled = cc->proc->body;
        cact_continue_frame_step(cc, CACT_JMP_EVAL_SEQ);
    }
}

