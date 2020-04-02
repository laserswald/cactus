#include <assert.h>

#include "cactus/core.h"
#include "cactus/sym.h"

#include "cactus/internal/debug.h"
#include "cactus/internal/utils.h"

void 
cact_mark_cont(struct cact_obj *o)
{
	struct cact_cont *c = (struct cact_cont *) o;

	cact_obj_mark((struct cact_obj *)c->env);
}

void
cact_destroy_cont(struct cact_obj *o)
{
	struct cact_cont *c = (struct cact_cont *) o;

	cact_obj_destroy((struct cact_obj *)c->env);
}

void
cact_init(struct cactus *cact)
{
	assert(cact != NULL);

	cact->gc_enabled = false;

	cact_store_init(&cact->store);
	cact_symbol_table_init(&cact->interned_syms);

    SLIST_INIT(&cact->conts);

	cact->root_env = (struct cact_env *) cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
	cact_env_init(cact->root_env, NULL);

	struct cact_cont *nc = (struct cact_cont *) cact_store_allocate(&cact->store, CACT_OBJ_CONT);
	nc->env = cact->root_env;

    SLIST_INSERT_HEAD(&cact->conts, nc, parent);

    ARRAY_INIT(&cact->preserved);

    cact->gc_enabled = true;
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

void
cact_preserve(struct cactus *cact, struct cact_obj *obj)
{
	ARRAY_ADD(&cact->preserved, obj);
}

void
cact_unpreserve(struct cactus *cact)
{
	ARRAY_REMOVE(&cact->preserved, ARRAY_LENGTH(&cact->preserved) - 1);
}

void
cact_collect_garbage(struct cactus *cact)
{
	struct cact_cont *c;

	DBG("; cactus: starting garbage collection... \n");

	DBG("; cactus gc: marking root environment \n");
	cact_obj_mark((struct cact_obj *) cact->root_env);

	DBG("; cactus gc: marking stack frames \n");
	SLIST_FOREACH(c, &cact->conts, parent) {
		DBG("; cactus gc: marking stack frame \n");
		cact_obj_mark((struct cact_obj *) c);
	}

	DBG("; cactus gc: marking preserved items \n"); 
	int i;
	for (i = 0; i < ARRAY_LENGTH(&cact->preserved); i++) {
		cact_obj_mark(ARRAY_ITEM(&cact->preserved, i));
	}

	int swept = cact_store_sweep(&cact->store);
	DBG("; cactus gc: swept %d objects \n", swept);
}

struct cact_obj *
cact_alloc(struct cactus *cact, enum cact_obj_type t)
{
    if (cact_store_needs_sweep(&cact->store) && cact->gc_enabled) { 
	    cact_collect_garbage(cact);
    }
	return cact_store_allocate(&cact->store, t);
}

void
cact_call_stack_push(struct cactus *cact, struct cact_env *frame)
{
	assert(cact != NULL);
	assert(frame != NULL);

	struct cact_cont *nc = (struct cact_cont *) cact_alloc(cact, CACT_OBJ_CONT);
	nc->env = frame;

	print_env(nc->env);

    SLIST_INSERT_HEAD(&cact->conts, nc, parent);
    puts("pushed call stack");
}

void
cact_call_stack_pop(struct cactus *cact)
{
	assert(cact != NULL);

    SLIST_REMOVE_HEAD(&cact->conts, parent);
    puts("popped call stack");
}

void
cact_show_call_stack(struct cactus *cact)
{
	struct cact_cont *c;

	DBG("; cactus: call stack:\n");
	SLIST_FOREACH(c, &cact->conts, parent) {
		DBG("; === call stack entry === :\n");
		print_env(c->env);
        puts("");
	}
}

struct cact_env *
cact_current_env(struct cactus *cact)
{
	assert(cact);

	if (SLIST_EMPTY(&cact->conts)) {
		return NULL;
	}

	return SLIST_FIRST(&cact->conts)->env;
}

/* Finalize a cactus interpreter. */
void
cact_finish(struct cactus *cact)
{
	// cact_store_finish(&cact->store);
}

