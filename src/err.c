#include "core.h"

#include "val.h"
#include "err.h"

/* Create a new heap-allocated error. */
struct cact_val
cact_make_error(struct cactus *cact, char *msg, struct cact_val irr)
{
    struct cact_error *err = (struct cact_error *)cact_store_allocate(&cact->store, CACT_OBJ_ERROR);
    err->msg = xstrdup(msg); // strdup?
    err->ctx = irr;
    return CACT_OBJ_VAL((struct cact_obj *)err);
}

/* Mark an error as reachable. */
void
cact_mark_error(struct cact_obj *o)
{
    struct cact_error *err = (struct cact_error *) o;
    cact_mark_val(err->ctx);
}

/* Free the error's data, but not the error itself. */
void
cact_destroy_error(struct cact_obj *o)
{
    struct cact_error *err = (struct cact_error *) o;
    xfree(err->msg);
    cact_destroy_val(err->ctx);
}

/* Raise an exception. */
struct cact_val
cact_raise(struct cactus *cact, struct cact_val exn)
{
    struct cact_cont *current = SLIST_FIRST(&cact->conts);

    while (current && ! current->exn_handler) {
        current = SLIST_NEXT(current, parent);
    }

    if (! current) {
        /*
         * Technically we shouldn't need this since we install the default exception, but
         * we might as well have this for safety
         */
        fprintf(stderr, "Uncaught exception");
        abort();
    }

    return cact_proc_apply(cact, current->exn_handler, exn);
}

