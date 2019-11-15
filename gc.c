#include "sexp.h"

struct cact_gc {
    struct cact_val_slist all;
    struct cact_val_slist protected;
};

/* Protect a sexp from collection. */
void 
cact_gc_protect(struct cact_gc *gc, struct sexp *x)
{
    SLIST_PUSH();
}

/* Allow a sexp to be collected. */
void 
cact_gc_yield(struct cact_gc *, struct sexp *)
{
    SLIST_PUSH();
}



/* Allocate a new sexp. */
struct sexp *
cact_gc_alloc(struct cact_gc *)
{

}

