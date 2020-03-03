#include "bool.h"

cact_val
cact_bool_not(cact_val x) 
{
	return CACT_BOOL_VAL(cact_is_bool(x) && x.as.boolean == false);
}
