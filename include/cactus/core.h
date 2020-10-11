#ifndef __CACT_CORE_H__
#define __CACT_CORE_H__

#include <stdio.h>

#include "cactus/env.h"
#include "cactus/read.h"
#include "cactus/sym.h"
#include "cactus/proc.h"

#include "cactus/internal/queue.h"

/**
 * A structure for quickly adding new native functions.
 */
struct cact_builtin {
	char* name;
	cact_native_func fn;
};

/*
 * A stack frame. We allocate these on the heap and use a linked list in the 
 * interpreter to keep track of them.
 */
struct cact_cont {
	struct cact_obj obj;
	struct cact_env *env;
	struct cact_proc *exn_handler;
	SLIST_ENTRY(cact_cont) parent;
};
SLIST_HEAD(cact_cont_stack, cact_cont);

/*
 * Initialize the continuation.
 */
void cact_cont_init(struct cact_cont *cont, struct cact_env *frame, struct cact_proc *exnh);

/*
 * Mark the continuation as being used.
 */
void cact_mark_cont(struct cact_obj *);

/*
 * 
 */
void cact_destroy_cont(struct cact_obj *);

#include "cactus/store.h"

ARRAY_DECL(cact_obj_vec, struct cact_obj *);

/**
 * The core structure for a Cactus interpreter.
 */
struct cactus {
	struct cact_env         *root_env;
	struct cact_cont_stack   conts;
	struct cact_lexer        lexer;
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

/* Define a native procedure in the global default namespace */
void cact_define_builtin(struct cactus *, const char *, cact_native_func);

/* Define a set of procedures in the global default namespace */
void cact_define_builtin_array(struct cactus *cact, struct cact_builtin *builtins, size_t len);

/* Allocate a new object from the heap. */
struct cact_obj *cact_alloc(struct cactus *, enum cact_obj_type);

/* Collect the garbage from the heap. */
int cact_collect_garbage(struct cactus *);

struct cact_cont *cact_current_cont(struct cactus *);
struct cact_env *cact_current_env(struct cactus *);
struct cact_proc *cact_current_exception_handler(struct cactus *);

void cact_call_stack_push(struct cactus *, struct cact_env *);
void cact_call_stack_pop(struct cactus *);
void cact_show_call_stack(struct cactus *);

bool cact_preserve(struct cactus *, struct cact_val);
bool cact_unpreserve(struct cactus *, struct cact_val);

#endif // __CACT_CORE_H__

