#include <setjmp.h>

#include "cactus/cont.h"

#include "cactus/core.h"
#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/obj.h"
#include "cactus/eval.h"

#include "cactus/internal/utils.h"
#include "cactus/internal/debug.h"
#include "cactus/internal/array.h"

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
    cont->state = CACT_JMP_FINISH;
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

    cact_mark_val(c->argl);
    cact_mark_val(c->expr);
    cact_mark_val(c->retval);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_destroy((struct cact_obj *)c->env);
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
    case CACT_JMP_APPLY:
	    return "apply";
    case CACT_JMP_IF_DECISION:
	    return "if_decision";
    case CACT_JMP_ASSIGN:
	    return "assign";
    case CACT_JMP_DEFINE:
	    return "define";
    case CACT_JMP_FINISH:
	    return "finish";
	}
}


/**
 * Finish the evaluation of the given continuation. 
 */
void
cact_finish_cont(struct cactus *cact, struct cact_cont *cont)
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
    cact_continue_cont(parent);
}

/**
 * Resume the continuation. Note that continuations need not be paused in order to resume them.
 * 
 * Starting from the current state, this will perform each action needed to continue evaluation.
 */
void
cact_resume_cont(struct cactus *cact, struct cact_cont *cont)
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
	    cact_finish_cont(cact, cont);
	    break;

    default:
        die("cact_cont_start: got mystery jump return value");
    }
}

void
cact_continue_cont(struct cact_cont *cont)
{
	longjmp(cont->bounce, cont->state);
}

void
cact_cont_continue_step(struct cact_cont *cont, enum cact_cont_state state)
{
	cont->state = state;
	cact_continue_cont(cont);
}

void
cact_cont_return(struct cact_cont *cont, struct cact_val value)
{
	cont->retval = value;
	cact_cont_continue_step(cont, CACT_JMP_FINISH);
}

