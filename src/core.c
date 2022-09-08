#include <assert.h>

#include "core.h"
#include "write.h"
#include "builtin.h"
#include "sym.h"

#include "internal/debug.h"
#include "internal/utils.h"

cact_value_t
cact_default_exception_handler(cact_context_t *cact, cact_value_t args);

/*
 * Initialize the Cactus interpreter.
 */
void
cact_init(cact_context_t *cact)
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
     * frames) that make a full record of the computation in progress. They
     * also store the current exception handler.
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
    cact->root_env = (cact_env_t *) cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
    cact_env_init(cact->root_env, NULL);

    /* Init the toplevel continuation. */
    cact_frame_t *nc = (cact_frame_t *) cact_store_allocate(&cact->store, CACT_OBJ_CONT);
    cact_init_frame(nc, cact->root_env, NULL);

    cact_procedure_t *default_exception_handler = cact_to_procedure(
                cact_make_native_proc(cact, cact_default_exception_handler),
                "initialization"
            );

    nc->exn_handler = default_exception_handler;

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
cact_finish(cact_context_t *cact)
{
    cact->gc_enabled = false;

    if (! ARRAY_EMPTY(&cact->preserved)) {
        /* Finish the preserved object stack */
        ARRAY_FREE(&cact->preserved);
    }

    cact_store_finish(&cact->store);
}

/* Allocate a new cactus interpreter off the heap. */
cact_context_t *
cact_create()
{
    cact_context_t *cact;

    cact = xcalloc(1, sizeof(*cact));
    cact_init(cact);

    return cact;
}

/* Release a heap-allocated interpreter back to the heap. */
void
cact_destroy(cact_context_t *cact)
{
    cact_finish(cact);

    xfree(cact);
}

/*
 * Define new bindings in the toplevel environment.
 */

/* Define a binding in the toplevel with the given value. */
void
cact_define(cact_context_t *cact, const char *name, cact_value_t val)
{
    cact_symbol_t *sym = cact_get_symbol(cact, name);
    cact_env_define(cact, cact->root_env, sym, val);
}

/* Define a builtin function, implemented in C. */
void
cact_define_builtin(cact_context_t *cact, const char *name, cact_native_func_t fn)
{
    cact_define(cact, name, cact_make_native_proc(cact, fn));
}

/* Define an array of builtins at once. */
void
cact_define_builtin_array(cact_context_t *cact, cact_builtin_t *builtins, size_t len)
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
cact_preserve(cact_context_t *cact, cact_value_t val)
{
    if (cact_is_obj(val)) {
        ARRAY_ADD(&cact->preserved, val.as.object);
        return true;
    }
    return false;
}

/* Withdraw preservation of an object from garbage collection. */
bool
cact_unpreserve(cact_context_t *cact, cact_value_t val)
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

    return false;
}

/*
 * Invoke the garbage collector, returning the amount of swept data.
 */
int
cact_collect_garbage(cact_context_t *cact)
{
    cact_frame_t *c;

    // DBG("; cactus: starting garbage collection... \n");

    // DBG("; cactus gc: marking root environment \n");
    cact_mark_object((cact_object_t *) cact->root_env);

    // DBG("; cactus gc: marking stack frames \n");
    SLIST_FOREACH(c, &cact->conts, parent) {
        // DBG("; cactus gc: marking stack frame \n");
        cact_mark_object((cact_object_t *) c);
    }

    // DBG("; cactus gc: marking preserved items \n");
    int i;
    for (i = 0; i < ARRAY_LENGTH(&cact->preserved); i++) {
        cact_mark_object(ARRAY_ITEM(&cact->preserved, i));
    }

    int swept = cact_store_sweep(&cact->store);
    // DBG("; cactus gc: swept %d objects \n", swept);

    return swept;
}

/*
 * Allocate a new object of a given type.
 */
cact_object_t *
cact_alloc(cact_context_t *cact, cact_object_type_t t)
{
    assert(cact != NULL);

    if (cact_store_needs_sweep(&cact->store) && cact->gc_enabled) {
        cact_collect_garbage(cact);
    }

    return cact_store_allocate(&cact->store, t);
}

/*
 * Finish a function call or macro expansion.
 */
void
cact_continuation_pop(cact_context_t *cact)
{
    assert(cact);

    SLIST_REMOVE_HEAD(&cact->conts, parent);
}

/* Show the current call stack. */
void
cact_show_call_stack(cact_context_t *cact)
{
    cact_frame_t *c;

    assert(cact);

    DBG("; cactus: call stack:\n");
    SLIST_FOREACH(c, &cact->conts, parent) {
        DBG("; === call stack entry === :\n");
        printf("%s\n", cact_show_frame_state(c->state));
        DBG("; === environment === :\n");
        print_env(c->env);
        DBG("; === retval === :\n");
        cact_display(c->retval);
        puts("");
        DBG("; === expression === :\n");
        cact_display(c->expr);
        puts("");
    }
}

/* Get the current continuation. */
cact_frame_t *
cact_current_frame(cact_context_t *cact)
{
    if (SLIST_EMPTY(&cact->conts)) {
        die("Did not expect no continuation!");
        return NULL;
    }

    return SLIST_FIRST(&cact->conts);
}


/* Get the current environment bindings. */
cact_env_t *
cact_current_env(cact_context_t *cact)
{
    assert(cact);

    cact_frame_t *cnt = cact_current_frame(cact);
    if (! cnt) {
        return NULL;
    }

    while (cnt->env == NULL) {
        cnt = SLIST_NEXT(cnt, parent);
    }

    return cnt->env;
}

cact_procedure_t *
cact_current_exception_handler(cact_context_t *cact)
{
    assert(cact);

    cact_frame_t *cnt;

    SLIST_FOREACH(cnt, &cact->conts, parent) {
        if (cnt->exn_handler) {
            return cnt->exn_handler;
        }
    }

    assert(false);
    return NULL;
}

cact_value_t
cact_default_exception_handler(cact_context_t *cact, cact_value_t args)
{
    fprintf(stderr, "Error!\n");
    cact_fdisplay(stderr, args);
    cact_show_call_stack(cact);
    abort();
}

cact_value_t
cact_current_retval(cact_context_t *cact)
{
    assert(cact);

    cact_frame_t *cnt = cact_current_frame(cact);

    if (! cnt) {
        return CACT_UNDEF_VAL;
    }

    return cnt->retval;
}

void
cact_continuation_push(cact_context_t *cact, cact_frame_t *cont)
{
    SLIST_INSERT_HEAD(&cact->conts, cont, parent);
}

