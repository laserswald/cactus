#include "cactus/core.h"
#include "cactus/sym.h"

#include "cactus/internal/debug.h"
#include "cactus/internal/utils.h"

void
cact_init(struct cactus *cact)
{
	// Begin by initializing the symbol table
	cact_symbol_table_init(&cact->interned_syms);
	cact_store_init(&cact->store);
	cact->root_env = malloc(sizeof(struct cact_env));
	cact_env_init(cact->root_env, NULL);
	cact->current_env = cact->root_env;
}

void
cact_define(struct cactus *cact, const char *name, struct cact_val val)
{
	struct cact_symbol *sym = cact_get_symbol(cact, name);
	cact_env_define(cact, cact->root_env, sym, val);
}

void
cact_define_builtin(struct cactus *cact, const char *name, cact_native_func fn)
{
	cact_define(cact, name, cact_make_native_proc(cact, fn));
}

void
cact_define_builtin_array(struct cactus *cact, struct cact_builtin *builtins, size_t len) 
{
	size_t i;
	for (i = 0; i < len; i++) {
		cact_define_builtin(cact, builtins[i].name, builtins[i].fn);
	}
}

/* Finalize a cactus interpreter. */
void
cact_finish(struct cactus *cact)
{
	// cact_store_finish(&cact->store);
}

