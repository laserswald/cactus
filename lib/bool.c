#include "cactus/bool.h"

bool
cact_is_truthy(struct cact_val x)
{
	return ! cact_is_bool(x) || cact_to_bool(x, "truthy?");
}

struct cact_val
cact_bool_not(struct cact_val x)
{
	return CACT_BOOL_VAL(cact_is_bool(x) && x.as.boolean == false);
}
