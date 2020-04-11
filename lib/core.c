#include <assert.h>

#include "cactus/core.h"
#include "cactus/sym.h"

#include "cactus/internal/debug.h"
#include "cactus/internal/utils.h"

/*
 * Initialize the Cactus interpreter.
 */
void
cact_init(struct cactus *cact)
{
    assert(cact != NULL);

    /*
     * Before we do anything, we make sure the GC is disabled for now. It
     * could seriously mess stuff up.
     */
    cact->gc_enabled = false;

    /*
     * Get the memory system set up, because we need it to allocate
     * procedures and continuations (what we call stack frames here).
     */
    cact_store_init(&cact->store);

    /*
     * Set up the global symbol table. This sets up the red-black tree for
     * symbol lookup (see sym.c for more details on why not a hash table)
     */
    cact_symbol_table_init(&cact->interned_syms);

    /*
     * Set up the continuation and environment stacks.
     *
     * The continuation stack is the set of partial continuations (e.g. stack
     * frames) that make a full record of the computation in progress.
     *
     * The environment stack is the set of all bindings that are visible at a
     * given point in the execution of an expression.
     */

    /* Init the continuation stack. */
    SLIST_INIT(&cact->conts);

    /*
     * Init the root environment.
     *
     * This is the environment that builtins will be inserted into. It will
     * never be garbage collected.
     */
    cact->root_env = (struct cact_env *) cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
    cact_env_init(cact->root_env, NULL);

    /* Init the toplevel continuation. */
    struct cact_cont *nc = (struct cact_cont *) cact_store_allocate(&cact->store, CACT_OBJ_CONT);
    nc->env = cact->root_env;
    SLIST_INSERT_HEAD(&cact->conts, nc, parent);

    /* Init the preserved object stack. */
    ARRAY_INIT(&cact->preserved);

    /* Don't forget the GC! */
    cact->gc_enabled = true;
}

/*
 * Finalize a cactus interpreter.
 */
void
cact_finish(struct cactus *cact)
{
    cact->gc_enabled = false;

    if (! ARRAY_EMPTY(&cact->preserved)) {
        /* Finish the preserved object stack */
        ARRAY_FREE(&cact->preserved);
    }

    cact_store_finish(&cact->store);
}

/* Allocate a new cactus interpreter off the heap. */
struct cactus *
cact_create()
{
    struct cactus *cact;

    cact = xcalloc(1, sizeof(*cact));
    cact_init(cact);

    return cact;
}

/* Release a heap-allocated interpreter back to the heap. */
void
cact_destroy(struct cactus *cact)
{
    cact_finish(cact);

    xfree(cact);
}

/*
 * Define new bindings in the toplevel environment.
 */

/* Define a binding in the toplevel with the given value. */
void
cact_define(struct cactus *cact, const char *name, struct cact_val val)
{
    struct cact_symbol *sym = cact_get_symbol(cact, name);
    cact_env_define(cact, cact->root_env, sym, val);
}

/* Define a builtin function, implemented in C. */
void
cact_define_builtin(struct cactus *cact, const char *name, cact_native_func fn)
{
    cact_define(cact, name, cact_make_native_proc(cact, fn));
}

/* Define an array of builtins at once. */
void
cact_define_builtin_array(struct cactus *cact, struct cact_builtin *builtins,
                          size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        cact_define_builtin(cact, builtins[i].name, builtins[i].fn);
    }
}

/*
 * Manage memory in the interpreter.
 */

/* Preserve a value from garbage collection. */
bool
cact_preserve(struct cactus *cact, struct cact_val val)
{
    if (cact_is_obj(val)) {
        ARRAY_ADD(&cact->preserved, val.as.object);
        return true;
    }
    return false;
}

/* Withdraw preservation of an object from garbage collection. */
bool
cact_unpreserve(struct cactus *cact, struct cact_val val)
{
    if (! cact_is_obj(val)) {
        return false;
    }

    size_t i;

    for (i = 0; i < ARRAY_LENGTH(&cact->preserved); i++) {
        if ((ARRAY_ITEM(&cact->preserved, i)) == val.as.object) {
            ARRAY_REMOVE(&cact->preserved, i);
            return true;
        }
    }

    assert(false && "Unmatched unpreserve: too many unpreserves!");
}

/* Invoke the garbage collector. */
void
cact_collect_garbage(struct cactus *cact)
{
    struct cact_cont *c;

    // DBG("; cactus: starting garbage collection... \n");

    // DBG("; cactus gc: marking root environment \n");
    cact_obj_mark((struct cact_obj *) cact->root_env);

    // DBG("; cactus gc: marking stack frames \n");
    SLIST_FOREACH(c, &cact->conts, parent) {
        // DBG("; cactus gc: marking stack frame \n");
        cact_obj_mark((struct cact_obj *) c);
    }

    // DBG("; cactus gc: marking preserved items \n");
    int i;
    for (i = 0; i < ARRAY_LENGTH(&cact->preserved); i++) {
        cact_obj_mark(ARRAY_ITEM(&cact->preserved, i));
    }

    int swept = cact_store_sweep(&cact->store);
    // DBG("; cactus gc: swept %d objects \n", swept);
}

/* Allocate a new object of a given type. */
struct cact_obj *
cact_alloc(struct cactus *cact, enum cact_obj_type t)
{
    assert(cact != NULL);

    if (cact_store_needs_sweep(&cact->store) && cact->gc_enabled) {
        cact_collect_garbage(cact);
    }

    return cact_store_allocate(&cact->store, t);
}

/* Record a new function call or macro expansion. */
void
cact_call_stack_push(struct cactus *cact, struct cact_env *frame)
{
    struct cact_cont *nc;

    assert(cact);
    assert(frame);

    nc = (struct cact_cont *) cact_alloc(cact, CACT_OBJ_CONT);
    nc->env = frame;

    SLIST_INSERT_HEAD(&cact->conts, nc, parent);
}

/* Finish a function call or macro expansion. */
void
cact_call_stack_pop(struct cactus *cact)
{
    assert(cact);

    SLIST_REMOVE_HEAD(&cact->conts, parent);
}

/* Show the current call stack. */
void
cact_show_call_stack(struct cactus *cact)
{
    struct cact_cont *c;

    assert(cact);

    DBG("; cactus: call stack:\n");
    SLIST_FOREACH(c, &cact->conts, parent) {
        DBG("; === call stack entry === :\n");
        print_env(c->env);
        puts("");
    }
}

/* Get the current environment bindings. */
struct cact_env *
cact_current_env(struct cactus *cact)
{
    assert(cact);

    if (SLIST_EMPTY(&cact->conts)) {
        return NULL;
    }

    return SLIST_FIRST(&cact->conts)->env;
}

/* Mark a continuation to be protected from garbage collection. */
void
cact_mark_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_mark((struct cact_obj *)c->env);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_destroy((struct cact_obj *)c->env);
}
