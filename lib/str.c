#include "cactus/str.h"

/* Create a string. */
struct cact_val *
cact_make_string(struct cactus *cact, char *str)
{
	struct cact_val v;

	v.type = CACT_TYPE_OBJ;
	v.as.object = cact_store_allocate(&cact->store);
	v.as.object->as.str.str = str;

	return v;
}

