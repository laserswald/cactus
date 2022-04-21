#include <setjmp.h>

#include "cont.h"

#include "core.h"
#include "proc.h"
#include "env.h"

#include "storage/obj.h"
#include "evaluator/eval.h"
#include "internal/utils.h"
#include "internal/debug.h"
#include "internal/array.h"

/*
 * Initialize a continuation frame.
 */
void
cact_cont_init(struct cact_cont *cont, struct cact_env *env, struct cact_proc *proc)
{
    assert(cont);

    cont->env = env;
    cont->exn_handler = NULL;

    cont->proc = proc;
    cont->argl = CACT_UNDEF_VAL;
    cont->expr = CACT_UNDEF_VAL;
    cont->unevaled = CACT_NULL_VAL;
    cont->retval = CACT_UNDEF_VAL;
    cont->state = CACT_JMP_EVAL_SINGLE;
}

void
cact_cont_init_load(struct cact_cont *cont, struct cact_env *env, char *string)
{
    cact_cont_init(cont, env, NULL);
    cact_lexer_init(&cont->lexer, string);
    cont->state = CACT_JMP_LOAD;
}

/*
 * Mark a continuation to be protected from garbage collection.
 */
void
cact_mark_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_mark((struct cact_obj *)c->env);
    cact_obj_mark((struct cact_obj *)c->exn_handler);
    cact_obj_mark((struct cact_obj *)c->proc);
    if (SLIST_NEXT(c, parent)) {
        cact_obj_mark((struct cact_obj *) SLIST_NEXT(c, parent));
    }
    cact_mark_val(c->argl);
    cact_mark_val(c->expr);
    cact_mark_val(c->retval);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_cont(struct cact_obj *o)
{
    return;
}

const char *
cact_cont_show_state(enum cact_cont_state state)
{
    switch (state) {
    case CACT_JMP_EVAL_SINGLE:
        return "eval_single";
    case CACT_JMP_EVAL_SEQ:
        return "eval_seq";
    case CACT_JMP_APPLY_DID_OP:
        return "apply_did_op";
    case CACT_JMP_ARG_POP:
        return "arg_pop";
    case CACT_JMP_BIND_ARG:
        return "bind_arg";
    case CACT_JMP_EXTEND_ENV:
        return "extend_env";
    case CACT_JMP_APPLY:
        return "apply";
    case CACT_JMP_IF_DECISION:
        return "if_decision";
    case CACT_JMP_ASSIGN:
        return "assign";
    case CACT_JMP_DEFINE:
        return "define";
    case CACT_JMP_LOAD:
        return "load";
    }
}


/**
 * Finish the evaluation of the given continuation.
 */
void
cact_cont_complete(struct cactus *cact, struct cact_cont *cont)
{
    struct cact_cont *parent = SLIST_NEXT(cont, parent);

    if (! parent) {
        return;
    }

    // Move the value from this continuation to the parent continuation
    parent->retval = cont->retval;

    // Pop this continuation off the stack
    cact_call_stack_pop(cact);

    // Resume the parent continuation
    cact_cont_continue(parent);
}

void
cact_cont_do_step(struct cactus *cact, struct cact_cont *cont);

/*
 * Resume the continuation. Note that continuations need not be paused in order to resume them.
 *
 * Starting from the current state, this will perform each action needed to continue evaluation.
 */
void
cact_cont_start(struct cactus *cact, struct cact_cont *cont)
{
    switch (setjmp(cont->bounce)) {
    case CACT_BOUNCE_START:
    case CACT_BOUNCE_CONTINUE:
        cact_cont_do_step(cact, cont);
        break;

    case CACT_BOUNCE_FINISH:
        cact_cont_complete(cact, cont);
        break;

    default:
        die("cact_cont_start: Got mysterious bounce value.");
    }
}


void
cact_cont_do_step(struct cactus *cact, struct cact_cont *cont)
{
    cont->is_started = true;
    switch (cont->state) {
    case CACT_JMP_EVAL_SINGLE:
        cact_eval_prim(cact, cont);
        break;

    case CACT_JMP_EVAL_SEQ:
        cact_eval_sequence(cact, cont);
        break;

    case CACT_JMP_APPLY_DID_OP:
        cact_eval_apply_with_operator(cact, cont);
        break;

    case CACT_JMP_ARG_POP:
        cact_eval_arg_pop(cact, cont);
        break;

    case CACT_JMP_BIND_ARG:
        cact_eval_arg_bind(cact, cont);
        break;

    case CACT_JMP_EXTEND_ENV:
        cact_eval_extend_env(cact, cont);
        break;

    case CACT_JMP_APPLY:
        cact_eval_apply(cact, cont);
        break;

    case CACT_JMP_IF_DECISION:
        cact_eval_branch(cact, cont);
        break;

    case CACT_JMP_ASSIGN:
        cact_eval_assignment(cact, cont);
        break;

    case CACT_JMP_DEFINE:
        cact_eval_definition(cact, cont);
        break;

    case CACT_JMP_LOAD:
        cact_eval_include(cact, cont);
        break;

    default:
        die("cact_cont_do_step: got mystery jump return value %d\n", cont->state);
    }
}

void
cact_cont_continue(struct cact_cont *cont)
{
    assert(cont->is_started);
    longjmp(cont->bounce, CACT_BOUNCE_CONTINUE);
}

void
cact_cont_step(struct cact_cont *cont, enum cact_cont_state state)
{
    cont->state = state;
    cact_cont_continue(cont);
}

void
cact_cont_finish(struct cact_cont *cont)
{
    assert(cont->is_started);
    longjmp(cont->bounce, CACT_BOUNCE_FINISH);
}

void
cact_cont_return(struct cact_cont *cont, struct cact_val value)
{
    cont->retval = value;
    cact_cont_finish(cont);
}


