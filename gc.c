#include "array.h"
#include "bitset.h"
#include "sexp.h"

const unsigned int CACT_GC_ALLOC_TRIGGER = 512;

struct cact_gc_handle {
    int marking; 
    SLIST_ENTRY(cact_gc_handle) entry;
};

SLIST_HEAD(cact_gc_handle_list, cact_gc_handle);

struct cact_gc {
	unsigned int alloc_count;
	struct cact_gc_handle_list roots;
	struct cact_gc_handle_list all;
};

void cact_gc_init(struct cact_gc *gc) {
	SLIST_INIT(gc->roots);
	SLIST_INIT(gc->all);
}

/* Protect a sexp from collection. */
void 
cact_gc_protect(struct cact_gc *gc, struct sexp *x) 
{
}

/* Allow a sexp to be collected. */
void 
cact_gc_yield(struct cact_gc *gc, struct sexp *x) 
{
}

void
cact_gc_mark_visible(struct cact_gc *gc)
{
	struct cact_gc_handle *current;
    SLIST_FOREACH(current, gc->roots, entry) {
        cact_val_mark((cact_val*) current);
    }
}

void
cact_gc_sweep(struct cact_gc *gc)
{
    struct cact_gc_handle *current, *deletable;
    deletable = NULL;
    SLIST_FOREACH_SAFE(current, gc->roots, entry, deletable) {
	    if (deletable && ! deletable->marking) {
            SLIST_REMOVE_AFTER(current, entry);
            fprintf(stderr, ";;;; gc: deleting item");
            xfree(deletable);
	    }
    }
}

/* Allocate a new sexp. */
struct sexp *
cact_gc_alloc(struct cact_gc *gc) {
	gc->alloc_count++;
    if (gc->alloc_count == CACT_GC_ALLOC_TRIGGER) {
	    cact_gc_sweep(gc);
	    gc->alloc_count = 0;
    }
    struct sexp *x = xmalloc(sizeof(struct sexp));
    SLIST_INSERT_HEAD(gc->all, x->gc_handle, entry);
}

