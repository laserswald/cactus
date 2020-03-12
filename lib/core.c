#include "cactus/core.h"

#include "cactus/internal/debug.h"
#include "cactus/internal/utils.h"

void
cact_init(struct cactus *cact)
{
	// Begin by initializing the symbol table
	TABLE_INIT(&cact->interned_syms, table_string_hash, table_string_cmp);

	cact_store_init(&cact->store);

	cact->root_env = malloc(sizeof(struct cact_env));
	cact_env_init(cact->root_env, NULL);

	cact->current_env = cact->root_env;
}

void
cact_def_global(struct cactus *cact, char *name, struct cact_val val)
{
    struct cact_val sym = cact_get_symbol(cact, name);
    cact_env_define(cact, cact->root_env, sym, val);
}

void
cact_make_builtins(struct cactus *cact, struct cact_builtin *builtins, size_t len) 
{
    size_t i;
    for (i = 0; i < len; i++) {
	    struct cact_val builtin = cact_make_native_proc(cact, builtins[i].fn);
	    cact_def_global(cact, builtins[i].name, builtin);
    }
}

/* Finalize a cactus interpreter. */
void
cact_finish(struct cactus *cact)
{
	// cact_store_finish(&cact->store);
}

