#ifndef NUMBER_H
#define NUMBER_H

#include "val.h"
#include "builtin.h"

DEFINE_VALUE_CONV(CACT_TYPE_FLONUM,  double, cact_to_double, flonum)
DEFINE_VALUE_CONV(CACT_TYPE_FIXNUM,  int,    cact_to_long,   fixnum)
DEFINE_VALUE_CHECK(cact_is_flonum, CACT_TYPE_FLONUM)
DEFINE_VALUE_CHECK(cact_is_fixnum, CACT_TYPE_FIXNUM)

bool 
cact_is_number(struct cact_val);
BUILTIN_FUNC(cact_builtin_is_number);

bool 
cact_is_zero(struct cact_val);
BUILTIN_FUNC(cact_builtin_is_zero);

/*
 * Numeric comparisons.
 */

bool
cact_less_than(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_less_than);

bool
cact_greater_than(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_greater_than);

bool
cact_less_or_equal(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_less_or_equal);

bool
cact_greater_or_equal(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_greater_or_equal);

bool
cact_num_equal(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_num_equal);

/*
 * Arithmetic.
 */

struct cact_val
cact_add(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_plus);

struct cact_val
cact_multiply(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_times);

struct cact_val
cact_subtract(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_minus);

struct cact_val
cact_divide(struct cactus *cact, struct cact_val a, struct cact_val b);
BUILTIN_FUNC(cact_builtin_divide);

#endif /* NUMBER_H */
