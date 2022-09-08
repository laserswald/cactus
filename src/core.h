#ifndef __CACT_CORE_H__
#define __CACT_CORE_H__

#include <stdio.h>

typedef struct cact_context cact_context_t;

#include "env.h"
#include "read.h"
#include "sym.h"
#include "proc.h"

#include "evaluator/cont.h"
#include "internal/queue.h"
#include "storage/store.h"

/**
 * The core structure for a Cactus interpreter.
 */
typedef struct cact_context {
	cact_env_t               *root_env;
	cact_continuation_t      conts;
	cact_lexer_t             lexer;
	cact_symbol_table_t      interned_syms;
	cact_store_t             store;
	cact_object_array_t      preserved;
	bool                     gc_enabled;
} cact_context_t;

/* Initialize a cactus interpreter. */
void 
cact_init(cact_context_t *cact);

/* Finalize a cactus interpreter. */
void 
cact_finish(cact_context_t *cact);

/* Define any value in the global default namespace */
void 
cact_define(cact_context_t *, const char *, cact_value_t);

/* Allocate a new object from the heap. */
cact_object_t*
cact_alloc(cact_context_t *, cact_object_type_t);

/* Collect the garbage from the heap. */
int 
cact_collect_garbage(cact_context_t *);

cact_env_t*
cact_current_env(cact_context_t *);

cact_procedure_t*
cact_current_exception_handler(cact_context_t *);

cact_value_t
cact_current_retval(cact_context_t *);

bool 
cact_preserve(cact_context_t *, cact_value_t);

bool 
cact_unpreserve(cact_context_t *, cact_value_t);

void 
cact_continue(cact_context_t *cact);

#endif // __CACT_CORE_H__

