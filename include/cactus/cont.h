#ifndef __CACTUS_CONT_H__
#define __CACTUS_CONT_H__

#include <setjmp.h>

#include "cactus/env.h"
#include "cactus/proc.h"

/*
 * A partial continuation. We allocate these on the heap and use a linked list in the 
 * interpreter to keep track of them.
 */
struct cact_cont {
	struct cact_obj obj;

	struct cact_env *env;
	struct cact_proc *proc;
	struct cact_val argl;
	struct cact_val evlist;
	struct cact_val retval;

	jmp_buf bounce;

	struct cact_proc *exn_handler;

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

/* Some constants for what happens when we get back from setjmp */
enum {
    CACT_JMP_INITIAL = 0, /* Set up the call and such */
    CACT_JMP_CALL, /* Tail call a new procedure */
    CACT_JMP_RETURN, /* Exit the continuation */
};

struct cact_val cact_resume_cont(struct cactus *cact, struct cact_cont *cont);

void cact_return(struct cactus *, struct cact_val);

void cact_call_stack_push(struct cactus *, struct cact_env *);
void cact_call_stack_pop(struct cactus *);
void cact_show_call_stack(struct cactus *);

struct cact_cont *cact_current_cont(struct cactus *);

#endif // __CACTUS_CONT_H__

