#include <assert.h>

#include "builtin.h"
#include "core.h"
#include "sym.h"
#include "write.h"

#include "internal/debug.h"
#include "internal/utils.h"

struct cact_val
cact_default_exception_handler(struct cactus *cact, struct cact_val args);

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
    cact->root_env = (struct cact_env *) cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
    cact_env_init(cact->root_env, NULL);

    /* Init the toplevel continuation. */
    struct cact_cont *nc = (struct cact_cont *) cact_store_allocate(&cact->store, CACT_OBJ_CONT);
    cact_cont_init(nc, cact->root_env, NULL);

    struct cact_proc *default_exception_handler =
        cact_to_procedure(cact_make_native_proc(cact, cact_default_exception_handler),
                          "initialization");

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

    return false;
}

/*
 * Invoke the garbage collector, returning the amount of swept data.
 */
int
cact_collect_garbage(struct cactus *cact)
{
    struct cact_cont *c;

    DBG("; cactus: starting garbage collection... \n");

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
    DBG("; cactus gc: swept %d objects \n", swept);

    return swept;
}

/*
 * Allocate a new object of a given type.
 */
struct cact_obj *
cact_alloc(struct cactus *cact, enum cact_obj_type t)
{
    assert(cact != NULL);

    if (cact_store_needs_sweep(&cact->store) && cact->gc_enabled) {
        DBG("; cactus: starting garbage collection... \n");
        cact_store_show(&cact->store);
        cact_collect_garbage(cact);
    }

    return cact_store_allocate(&cact->store, t);
}

/*
 * Finish a function call or macro expansion.
 */
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
        DBG("; frame: ");
        printf("%s\n", cact_cont_show_state(c->state));
        DBG("  ; retval: ");
        cact_display(c->retval);
        puts("");
        DBG("  ; expression: ");
        cact_display(c->expr);
        puts("");
        DBG("  ; unevaluated: ");
        cact_display(c->unevaled);
        puts("");
        DBG("  ; argl: ");
        cact_display(c->argl);
        puts("");
    }
}

/* Get the current continuation. */
struct cact_cont *
cact_current_cont(struct cactus *cact)
{
    if (SLIST_EMPTY(&cact->conts)) {
        die("Did not expect no continuation!");
        return NULL;
    }

    return SLIST_FIRST(&cact->conts);
}


/* Get the current environment bindings. */
struct cact_env *
cact_current_env(struct cactus *cact)
{
    assert(cact);

    struct cact_cont *cnt = cact_current_cont(cact);
    if (! cnt) {
        return NULL;
    }

    while (cnt->env == NULL) {
        cnt = SLIST_NEXT(cnt, parent);
    }

    return cnt->env;
}

struct cact_proc *
cact_current_exception_handler(struct cactus *cact)
{
    assert(cact);

    struct cact_cont *cnt;

    SLIST_FOREACH(cnt, &cact->conts, parent) {
        if (cnt->exn_handler) {
            return cnt->exn_handler;
        }
    }

    assert(false);
    return NULL;
}

struct cact_val
cact_default_exception_handler(struct cactus *cact, struct cact_val args)
{
    fprintf(stderr, "Error!\n");
    cact_fdisplay(stderr, args);
    cact_show_call_stack(cact);
    abort();
}

struct cact_val
cact_current_retval(struct cactus *cact)
{
    assert(cact);

    struct cact_cont *cnt = cact_current_cont(cact);

    if (! cnt) {
        return CACT_UNDEF_VAL;
    }

    return cnt->retval;
}

void
cact_call_stack_push(struct cactus *cact, struct cact_cont *cont)
{
    SLIST_INSERT_HEAD(&cact->conts, cont, parent);
}

