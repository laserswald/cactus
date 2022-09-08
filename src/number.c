#include "num.h"

#include "builtin.h"
#include "pair.h"
#include "err.h"

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_fixnum, cact_is_fixnum)

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_flonum, cact_is_flonum)

bool
cact_is_number(cact_value_t x)
{
    return cact_is_fixnum(x) || cact_is_flonum(x);
}

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_number, cact_is_number)

bool
cact_is_zero(cact_value_t x)
{
    if (cact_is_fixnum(x)) {
        return x.as.fixnum == 0;
    } else if (cact_is_flonum(x)) {
        return x.as.flonum == 0.0;
    } else {
        return false;
    }
}
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_zero, cact_is_zero)

cact_value_t
cact_to_inexact(cact_context_t *cact, cact_value_t x)
{
    if (cact_is_flonum(x)) {
        return x;
    } else if (cact_is_fixnum(x)) {
        return CACT_FLO_VAL(x.as.fixnum);
    } else {
        cact_raise(cact, cact_make_error(cact, "Not a number", x));
    }
    return CACT_UNDEF_VAL;
}

#define CACT_DEFINE_COMPARATOR(name, op) \
bool \
name(cact_context_t *cact, cact_value_t a, cact_value_t b) \
{ \
	if (cact_is_flonum(a) || cact_is_flonum(b)) { \
		cact_value_t fa, fb; \
		fa = cact_to_inexact(cact, a); \
		fb = cact_to_inexact(cact, b); \
 \
		return fa.as.flonum op fb.as.flonum; \
	} else { \
		return a.as.fixnum op b.as.fixnum; \
	} \
}

CACT_DEFINE_COMPARATOR(cact_less_than, <)
CACT_DEFINE_COMPARATOR(cact_greater_than, >)
CACT_DEFINE_COMPARATOR(cact_less_or_equal, <=)
CACT_DEFINE_COMPARATOR(cact_greater_or_equal, >=)
CACT_DEFINE_COMPARATOR(cact_num_equal, ==)

#undef CACT_DEFINE_COMPARATOR

#define CACT_DEFINE_BUILTIN_COMPARATOR(name, compare) \
cact_value_t \
name(cact_context_t *cact, cact_value_t args) \
{ \
	cact_value_t left = cact_car(cact, args); \
	cact_value_t rest = cact_cdr(cact, args); \
    CACT_LIST_FOR_EACH_ITEM(cact, arg, rest) { \
	    if (! compare(cact, left, arg)) { \
		    return CACT_BOOL_VAL(false); \
	    } \
	    left = arg; \
    } \
    return CACT_BOOL_VAL(true); \
}

CACT_DEFINE_BUILTIN_COMPARATOR(cact_builtin_less_than, cact_less_than)
CACT_DEFINE_BUILTIN_COMPARATOR(cact_builtin_greater_than, cact_greater_than)
CACT_DEFINE_BUILTIN_COMPARATOR(cact_builtin_less_or_equal, cact_less_or_equal)
CACT_DEFINE_BUILTIN_COMPARATOR(cact_builtin_greater_or_equal, cact_greater_or_equal)
CACT_DEFINE_BUILTIN_COMPARATOR(cact_builtin_num_equal, cact_num_equal)

#undef CACT_DEFINE_BUILTIN_COMPARATOR


#define CACT_DEFINE_BINARY_OP(name, op) \
cact_value_t \
name(cact_context_t *cact, cact_value_t a, cact_value_t b) \
{ \
	if (cact_is_flonum(a) || cact_is_flonum(b)) { \
		cact_value_t fa, fb; \
		fa = cact_to_inexact(cact, a); \
		fb = cact_to_inexact(cact, b); \
 \
		return CACT_FLO_VAL(fa.as.flonum op fb.as.flonum); \
	} else { \
		return CACT_FIX_VAL(a.as.fixnum op b.as.fixnum); \
	} \
}

CACT_DEFINE_BINARY_OP(cact_add, +)

cact_value_t
cact_builtin_plus(cact_context_t *cact, cact_value_t args)
{
    cact_value_t sum = CACT_FIX_VAL(0);
    CACT_LIST_FOR_EACH_ITEM(cact, addend, args) {
        sum = cact_add(cact, sum, addend);
    }
    return sum;
}

CACT_DEFINE_BINARY_OP(cact_multiply, *)

cact_value_t
cact_builtin_times(cact_context_t *cact, cact_value_t args)
{
    cact_value_t product = CACT_FIX_VAL(1);
    CACT_LIST_FOR_EACH_ITEM(cact, factor, args) {
        product = cact_multiply(cact, product, factor);
    }
    return product;
}

CACT_DEFINE_BINARY_OP(cact_subtract, -)

cact_value_t
cact_builtin_minus(cact_context_t *cact, cact_value_t x)
{
    cact_value_t initial = cact_car(cact, x);
    if (cact_is_null(cact_cdr(cact, x))) {
        return cact_subtract(cact, CACT_FIX_VAL(0), initial);
    } else {
        cact_value_t rest = cact_cdr(cact, x);
        CACT_LIST_FOR_EACH_ITEM(cact, subtrahend, rest) {
            initial = cact_subtract(cact, initial, subtrahend);
        }
        return initial;
    }
}

#undef CACT_DEFINE_BINARY_OP

#define CACT_DEFINE_DIVISION_OP(name, op) \
cact_value_t \
name(cact_context_t *cact, cact_value_t a, cact_value_t b) \
{ \
    if (cact_is_zero(b)) { \
        cact_raise(cact, cact_make_error(cact, "Division by zero", CACT_NULL_VAL)); \
    } \
	if (cact_is_flonum(a) || cact_is_flonum(b)) { \
		cact_value_t fa, fb; \
		fa = cact_to_inexact(cact, a); \
		fb = cact_to_inexact(cact, b); \
 \
		return CACT_FLO_VAL(fa.as.flonum op fb.as.flonum); \
	} else { \
		return CACT_FIX_VAL(a.as.fixnum op b.as.fixnum); \
	} \
}

CACT_DEFINE_DIVISION_OP(cact_divide, /)

cact_value_t
cact_builtin_divide(cact_context_t *cact, cact_value_t x)
{
    cact_value_t initial = cact_car(cact, x);
    if (cact_is_null(cact_cdr(cact, x))) {
        return cact_divide(cact, CACT_FIX_VAL(1), initial);
    } else {
        cact_value_t rest = cact_cdr(cact, x);
        CACT_LIST_FOR_EACH_ITEM(cact, divisor, rest) {
            initial = cact_divide(cact, initial, divisor);
        }
        return initial;
    }
}

#undef CACT_DEFINE_DIVISION_OP


