#ifndef __CACT_CORE_H__
#define __CACT_CORE_H__

#include <stdio.h>

#include "cactus/env.h"
#include "cactus/read.h"
#include "cactus/sym.h"
#include "cactus/store.h"
#include "cactus/proc.h"

/**
 * A structure for quickly adding new native functions.
 */
struct cact_builtin {
    char* name;
    cact_native_func fn;
};

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

/* Define any value in the global default namespace */
void cact_define(struct cactus *, const char *, struct cact_val);

/* Define a native procedure in the global default namespace */
void cact_define_builtin(struct cactus *, const char *, cact_native_func);

/* Define a set of procedures in the global default namespace */
void cact_define_builtin_array(struct cactus *cact, struct cact_builtin *builtins, size_t len);

#endif // __CACT_CORE_H__

