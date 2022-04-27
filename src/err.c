#include "core.h"

#include "val.h"
#include "err.h"

/* Create a new heap-allocated error. */
cact_value_t
cact_make_error(cact_context_t *cact, char *msg, cact_value_t irr)
{
    cact_error_t *err = (cact_error_t *)cact_store_allocate(&cact->store, CACT_OBJ_ERROR);
    err->msg = xstrdup(msg); // strdup?
    err->ctx = irr;
    return CACT_OBJ_VAL((cact_object_t *)err);
}

/* Mark an error as reachable. */
void
cact_mark_error(cact_object_t *o)
{
    cact_error_t *err = (cact_error_t *) o;
    cact_mark_value(err->ctx);
}

/* Free the error's data, but not the error itself. */
void
cact_destroy_error(cact_object_t *o)
{
    cact_error_t *err = (cact_error_t *) o;
    xfree(err->msg);
    cact_destroy_value(err->ctx);
}

/* Raise an exception. */
cact_value_t
cact_raise(cact_context_t *cact, cact_value_t exn)
{
    cact_frame_t *current = SLIST_FIRST(&cact->conts);

    while (! current->exn_handler && current) {
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

