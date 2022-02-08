#ifndef __CACTUS_CONT_H__
#define __CACTUS_CONT_H__

#include <setjmp.h>

#include "cactus/env.h"
#include "cactus/proc.h"
#include "cactus/internal/array.h"

enum cact_cont_state {
    CACT_JMP_START = 0,

    // This state will try to evaluate whatever is in the expr field and return it's 
    // value int
    CACT_JMP_EVAL_SINGLE,

    CACT_JMP_EVAL_SEQ,

    // If we found an application, we do these steps. First, we jump back when we want to 
    // get the current operand...
    CACT_JMP_APPLY_DID_OP,

    // Pop an argument off unevaled and evaluate it...
    CACT_JMP_ARG_POP,

    // And then accumulate it
    CACT_JMP_BIND_ARG,

    // Bind all the arguments into a new environment
    CACT_JMP_EXTEND_ENV,

    // And finally apply the procedure when we are done accumulating.
    CACT_JMP_APPLY,

    // Conditionals need some explicit support. Continuations in this state are waiting on 
    // the conditional of an if expression. When the inner conditional is done evaluating, this 
    // state will perform the branch.
    CACT_JMP_IF_DECISION,

    // Assignments are fairly easy; this will assign the value in 
    CACT_JMP_ASSIGN,

    // Defines a new 
    CACT_JMP_DEFINE,

    // And here after we are done with everything. Once this is done, the continuation's 
    // retval field will have the final evaluated state.
    CACT_JMP_FINISH,
};

/*
 * A partial continuation. We allocate these on the heap and use a linked list in the 
 * interpreter to keep track of them.
 */
struct cact_cont {
	struct cact_obj obj;

    // The evaluation environment.
	struct cact_env *env;

	// While we are applying the procedure, it is stored here.
	struct cact_proc *proc;

    // The list of arguments for the procedure (note: they must be evaluated!) 
    // will be here.
	struct cact_val argl;

    // The expression to evaluate.
	struct cact_val expr;

    // The list of expressions to evaluate.
	struct cact_val unevaled;

    // The return value of the evaluation is stored here.
	struct cact_val retval;

    // The state of the continuation is here. When resumed, this will be looked at
    // in order to perform the next actions while evaluating.
    enum cact_cont_state state;

    // This is the jump context that each step of the evaluator will jump back to.
	jmp_buf bounce;

	struct cact_proc *exn_handler;

	bool is_started;

	SLIST_ENTRY(cact_cont) parent;
};

SLIST_HEAD(cact_cont_stack, cact_cont);

/*
 * Initialize the continuation.
 */
void cact_cont_init(struct cact_cont *cont, struct cact_env *parent_env, struct cact_proc *proc);

/*
 * Mark the continuation as being used.
 */
void cact_mark_cont(struct cact_obj *);

/*
 * Destroy the continuation.
 */
void cact_destroy_cont(struct cact_obj *);

/*
 * Either begin a newly created continuation or resume a previously started one
 */
void cact_resume_cont(struct cactus *cact, struct cact_cont *cont);

/*
 * Begin the continuation. After this, the continuation is enabled and ready to 
 * be continued later.
 */
void cact_continue_cont(struct cact_cont *cc);
void cact_cont_return(struct cact_cont *cont, struct cact_val value);
void cact_cont_continue_step(struct cact_cont *cont, enum cact_cont_state state);

void cact_call_stack_push(struct cactus *, struct cact_cont *cont);
void cact_call_stack_pop(struct cactus *);
void cact_show_call_stack(struct cactus *);

const char* cact_cont_show_state(enum cact_cont_state state);

struct cact_cont *cact_current_cont(struct cactus *);

#endif // __CACTUS_CONT_H__

