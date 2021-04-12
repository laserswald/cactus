#ifndef __CACTUS_CONT_H__
#define __CACTUS_CONT_H__

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
void cact_cont_init(struct cact_cont *cont, struct cact_env *frame);

/*
 * Mark the continuation as being used.
 */
void cact_mark_cont(struct cact_obj *);

/*
 * Destroy the continuation.
 */
void cact_destroy_cont(struct cact_obj *);

struct cact_val
cact_start_cont(struct cactus *, struct cact_cont)

void // NORETURN
cact_return(struct cactus *, struct cact_val);

#endif // __CACTUS_CONT_H__

