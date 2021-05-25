#ifndef NUMBER_H
#define NUMBER_H

#include "cactus/val.h"

DEFINE_VALUE_CONV(CACT_TYPE_FLONUM,  double, cact_to_double, flonum)
DEFINE_VALUE_CONV(CACT_TYPE_FIXNUM,  int,    cact_to_long,   fixnum)
DEFINE_VALUE_CHECK(cact_is_flonum, CACT_TYPE_FLONUM)
DEFINE_VALUE_CHECK(cact_is_fixnum, CACT_TYPE_FIXNUM)

bool cact_is_number(struct cact_val);

bool cact_number_less(struct cact_val, struct cact_val);
bool cact_number_less_or_equal(struct cact_val, struct cact_val);
bool cact_number_greater(struct cact_val, struct cact_val);
bool cact_number_greater_or_equal(struct cact_val, struct cact_val);
bool cact_number_equal(struct cact_val, struct cact_val);

#endif /* NUMBER_H */
