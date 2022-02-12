#ifndef __CACT_CORE_H__
#define __CACT_CORE_H__

#include <stdio.h>

#include "env.h"
#include "read.h"
#include "sym.h"
#include "proc.h"
#include "evaluator/cont.h"

#include "internal/queue.h"


#include "storage/store.h"

ARRAY_DECL(cact_obj_vec, struct cact_obj *);

/**
 * The core structure for a Cactus interpreter.
 */
struct cactus {
	struct cact_env         *root_env;
	struct cact_cont_stack   conts;
	struct cact_symbol_table interned_syms;
	struct cact_store        store;
	struct cact_obj_vec      preserved;
	bool                     gc_enabled;
};

/* Initialize a cactus interpreter. */
void cact_init(struct cactus *);

/* Finalize a cactus interpreter. */
void cact_finish(struct cactus *);

/* Define any value in the global default namespace */
void cact_define(struct cactus *, const char *, struct cact_val);

/* Allocate a new object from the heap. */
struct cact_obj *cact_alloc(struct cactus *, enum cact_obj_type);

/* Collect the garbage from the heap. */
int cact_collect_garbage(struct cactus *);

struct cact_env *cact_current_env(struct cactus *);
struct cact_proc *cact_current_exception_handler(struct cactus *);
struct cact_val cact_current_retval(struct cactus *);

bool cact_preserve(struct cactus *, struct cact_val);
bool cact_unpreserve(struct cactus *, struct cact_val);

void cact_continue(struct cactus *cact);

#endif // __CACT_CORE_H__

