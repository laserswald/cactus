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
cact_init_frame(cact_frame_t *frame, cact_env_t *env, cact_procedure_t *proc)
{
    assert(frame);

    frame->env = env;
    frame->exn_handler = NULL;

    frame->proc = proc;
    frame->argl = CACT_UNDEF_VAL;
    frame->expr = CACT_UNDEF_VAL;
    frame->unevaled = CACT_NULL_VAL;
    frame->retval = CACT_UNDEF_VAL;
    frame->state = CACT_JMP_FINISH;
}

/*
 * Mark a continuation to be protected from garbage collection.
 */
void
cact_mark_frame(cact_object_t *o)
{
    assert(o);

    cact_frame_t *c = (cact_frame_t *) o;

    cact_mark_object((cact_object_t *)c->env);
    cact_mark_object((cact_object_t *)c->exn_handler);
    cact_mark_object((cact_object_t *)c->proc);
    cact_mark_object((cact_object_t *) SLIST_NEXT(c, parent));

    cact_mark_value(c->argl);
    cact_mark_value(c->expr);
    cact_mark_value(c->retval);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_frame(cact_object_t *o)
{
	return;
}

const char *
cact_show_frame_state(cact_frame_state_t state)
{
    switch (state) {
    case CACT_JMP_START:
        return "start";
    case CACT_JMP_EVAL_SINGLE:
        return "eval_single";
    case CACT_JMP_EVAL_SEQ:
        return "eval_seq";
    case CACT_JMP_APPLY_DID_OP:
        return "apply_did_op";
    case CACT_JMP_APPLY:
        return "apply";
    case CACT_JMP_ARG_POP:
        return "arg_pop";
    case CACT_JMP_BIND_ARG:
        return "bind_arg";
    case CACT_JMP_EXTEND_ENV:
        return "extend_env";
    case CACT_JMP_IF_DECISION:
        return "if_decision";
    case CACT_JMP_ASSIGN:
        return "assign";
    case CACT_JMP_DEFINE:
        return "define";
    case CACT_JMP_FINISH:
        return "finish";
    }

    return "UNKNOWN";
}


/**
 * Finish the evaluation of the given continuation.
 */
void
cact_finish_frame(cact_context_t *cact, cact_frame_t *cont)
{
    cact_frame_t *parent = SLIST_NEXT(cont, parent);

    if (! parent) {
        return;
    }

    // Move the value from this continuation to the parent continuation
    parent->retval = cont->retval;

    // Pop this continuation off the stack
    cact_continuation_pop(cact);

    // Resume the parent continuation
    cact_continue_frame(parent);
}

/**
 * Resume the continuation. Note that continuations need not be paused in order to resume them.
 *
 * Starting from the current state, this will perform each action needed to continue evaluation.
 */
void
cact_resume_frame(cact_context_t *cact, cact_frame_t *cont)
{
    switch (setjmp(cont->bounce)) {
    case CACT_JMP_START:
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

    case CACT_JMP_FINISH:
        cact_finish_frame(cact, cont);
        break;

    default:
        die("cact_cont_start: got mystery jump return value");
    }
}

void
cact_continue_frame(cact_frame_t *cont)
{
    longjmp(cont->bounce, cont->state);
}

void
cact_continue_frame_step(cact_frame_t *cont, cact_frame_state_t state)
{
    cont->state = state;
    cact_continue_frame(cont);
}

void
cact_leave_frame(cact_frame_t *cont, cact_value_t value)
{
    cont->retval = value;
    cact_continue_frame_step(cont, CACT_JMP_FINISH);
}

