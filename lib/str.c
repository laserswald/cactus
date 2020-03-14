#include "cactus/str.h"

#include "cactus/core.h"

/* Create a string. */
struct cact_val
cact_make_string(struct cactus *cact, const char *str)
{
	struct cact_string *s;

	s = (struct cact_string *)cact_store_allocate(&cact->store, CACT_OBJ_STRING);
	s->str = str;

	return CACT_OBJ_VAL(s);
}

