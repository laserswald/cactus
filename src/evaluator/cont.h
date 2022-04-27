#ifndef __CACTUS_CONT_H__
#define __CACTUS_CONT_H__

#include <setjmp.h>

#include "env.h"
#include "proc.h"
#include "internal/array.h"

typedef enum cact_frame_state {
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
} cact_frame_state_t;

const char * 
cact_show_frame_state(cact_frame_state_t state);

/*
 * A stack frame. A full list of these is the current continuation.
 */
typedef struct cact_frame {
	cact_object_t obj;

    // The evaluation environment.
	cact_env_t *env;

	// While we are applying the procedure, it is stored here.
	cact_procedure_t *proc;

    // The list of arguments for the procedure (note: they must be evaluated!) 
    // will be here.
	cact_value_t argl;

    // The expression to evaluate.
	cact_value_t expr;

    // The list of expressions to evaluate.
	cact_value_t unevaled;

    // The return value of the evaluation is stored here.
	cact_value_t retval;

    // The state of the continuation is here. When resumed, this will be looked at
    // in order to perform the next actions while evaluating.
    cact_frame_state_t state;

    // This is the jump context that each step of the evaluator will jump back to.
	jmp_buf bounce;

	cact_procedure_t *exn_handler;

	bool is_started;

	SLIST_ENTRY(cact_frame) parent;
} cact_frame_t;

SLIST_HEAD(cact_continuation, cact_frame);
typedef struct cact_continuation cact_continuation_t;

/*
 * Initialize the frame.
 */
void 
cact_init_frame(cact_frame_t *, cact_env_t *, cact_procedure_t *);

/*
 * Mark the frame as being used.
 */
void 
cact_mark_frame(cact_object_t *);

/*
 * Destroy the frame.
 */
void 
cact_destroy_frame(cact_object_t *);

/*
 * Either begin a newly created frame or resume a previously started one
 */
void 
cact_resume_frame(cact_context_t *cact, cact_frame_t *cont);

/*
 * Begin the continuation. After this, the continuation is enabled and ready to 
 * be continued later.
 */
void 
cact_continue_frame(cact_frame_t *cc);

void 
cact_leave_frame(cact_frame_t *cont, cact_value_t value);

void 
cact_continue_frame_step(cact_frame_t *cont, cact_frame_state_t state);

cact_frame_t *
cact_current_frame(cact_context_t *);

void 
cact_continuation_push(cact_context_t *, cact_frame_t *);

void 
cact_continuation_pop(cact_context_t *);

void 
cact_show_continuation(cact_context_t *);



#endif // __CACTUS_CONT_H__

