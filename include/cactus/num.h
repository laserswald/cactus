#ifndef NUMBER_H
#define NUMBER_H

#include "cactus/val.h"

DEFINE_VALUE_CONV(CACT_TYPE_FLONUM,  double, cact_to_double, flonum)
DEFINE_VALUE_CONV(CACT_TYPE_FIXNUM,  int,    cact_to_long,   fixnum)
DEFINE_VALUE_CHECK(cact_is_flonum, CACT_TYPE_FLONUM)
DEFINE_VALUE_CHECK(cact_is_fixnum, CACT_TYPE_FIXNUM)

bool cact_is_number(struct cact_val);

#endif /* NUMBER_H */
