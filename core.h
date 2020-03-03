#ifndef __CACT_CORE_H__
#define __CACT_CORE_H__

#include <stdio.h>

#include "sexp.h"
#include "sym.h"
#include "read.h"
#include "store.h"

/**
 * The core structure for a Cactus interpreter.
 */
struct cactus {
	struct cact_env *root_env;
	struct cact_env *current_env;
	struct cact_lexer lexer;
	struct cact_symbol_table interned_syms;
	struct cact_store store;
};

/* Initialize a cactus interpreter. */
void cact_init(struct cactus *);

/* Finalize a cactus interpreter. */
void cact_finish(struct cactus *);

void cact_def_global(struct cactus *, char *, struct cact_val);

#endif // __CACT_CORE_H__

