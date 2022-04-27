#ifndef NUMBER_H
#define NUMBER_H

#include "val.h"
#include "builtin.h"

DEFINE_VALUE_CONV(CACT_TYPE_FLONUM,  double, cact_to_double, flonum)
DEFINE_VALUE_CONV(CACT_TYPE_FIXNUM,  int,    cact_to_long,   fixnum)
DEFINE_VALUE_CHECK(cact_is_flonum, CACT_TYPE_FLONUM)
DEFINE_VALUE_CHECK(cact_is_fixnum, CACT_TYPE_FIXNUM)

bool 
cact_is_number(cact_value_t);
CACT_BUILTIN_FUNC(cact_builtin_is_number);

bool 
cact_is_zero(cact_value_t);
CACT_BUILTIN_FUNC(cact_builtin_is_zero);

/*
 * Numeric comparisons.
 */

bool
cact_less_than(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_less_than);

bool
cact_greater_than(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_greater_than);

bool
cact_less_or_equal(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_less_or_equal);

bool
cact_greater_or_equal(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_greater_or_equal);

bool
cact_num_equal(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_num_equal);

/*
 * Arithmetic.
 */

cact_value_t
cact_add(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_plus);

cact_value_t
cact_multiply(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_times);

cact_value_t
cact_subtract(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_minus);

cact_value_t
cact_divide(cact_context_t *cact, cact_value_t a, cact_value_t b);
CACT_BUILTIN_FUNC(cact_builtin_divide);

#endif /* NUMBER_*/
