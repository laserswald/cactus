#include "cactus/err.h"

#include "cactus/core.h"

struct cact_val
cact_make_error(struct cactus *cact, char *msg, struct cact_val irr)
{
	struct cact_error *err = (struct cact_error *)cact_store_allocate(&cact->store, CACT_OBJ_ERROR);
    return CACT_OBJ_VAL((struct cact_obj *)err);
}
