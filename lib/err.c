#include "cactus/err.h"

#include "cactus/core.h"

struct cact_val
cact_make_error(struct cactus *cact, char *msg, struct cact_val irr)
{
	struct cact_error *err = (struct cact_error *)cact_store_allocate(&cact->store, CACT_OBJ_ERROR);
	err->msg = msg; // strdup?
	err->ctx = irr;
	return CACT_OBJ_VAL((struct cact_obj *)err);
}

void
cact_mark_error(struct cact_obj *o)
{
	struct cact_error *err = (struct cact_error *) o;
	cact_mark_val(err->ctx);
}

void
cact_destroy_error(struct cact_obj *o)
{
	struct cact_error *err = (struct cact_error *) o;
	cact_mark_val(err->ctx);
}
