#include "cactus/num.h"

bool
cact_is_number(struct cact_val x)
{
    return cact_is_fixnum(x) || cact_is_flonum(x);
}
