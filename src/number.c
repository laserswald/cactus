#include "num.h"

#include "builtin.h"
#include "pair.h"
#include "err.h"

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_fixnum, cact_is_fixnum)

DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_flonum, cact_is_flonum)

bool
cact_is_number(struct cact_val x)
{
    return cact_is_fixnum(x) || cact_is_flonum(x);
}
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_number, cact_is_number)

bool
cact_is_zero(struct cact_val x)
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

struct cact_val
cact_to_inexact(struct cactus *cact, struct cact_val x)
{
    if (cact_is_flonum(x)) {
        return x;
    } else if (cact_is_fixnum(x)) {
        return CACT_FLO_VAL(x.as.fixnum);
    } else {
        cact_raise(cact, cact_make_error(cact, "Not a number", x));
    }
}

#define CACT_DEFINE_COMPARATOR(name, op) \
bool \
name(struct cactus *cact, struct cact_val a, struct cact_val b) \
{ \
	if (cact_is_flonum(a) || cact_is_flonum(b)) { \
		struct cact_val fa, fb; \
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
struct cact_val \
name(struct cactus *cact, struct cact_val args) \
{ \
	struct cact_val left = cact_car(cact, args); \
	struct cact_val rest = cact_cdr(cact, args); \
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
struct cact_val \
name(struct cactus *cact, struct cact_val a, struct cact_val b) \
{ \
	if (cact_is_flonum(a) || cact_is_flonum(b)) { \
		struct cact_val fa, fb; \
		fa = cact_to_inexact(cact, a); \
		fb = cact_to_inexact(cact, b); \
 \
		return CACT_FLO_VAL(fa.as.flonum op fb.as.flonum); \
	} else { \
		return CACT_FIX_VAL(a.as.fixnum op b.as.fixnum); \
	} \
}

CACT_DEFINE_BINARY_OP(cact_add, +)
CACT_DEFINE_BINARY_OP(cact_multiply, *)
CACT_DEFINE_BINARY_OP(cact_subtract, -)

#undef CACT_DEFINE_BINARY_OP

#define CACT_DEFINE_DIVISION_OP(name, op) \
struct cact_val \
name(struct cactus *cact, struct cact_val a, struct cact_val b) \
{ \
    if (cact_is_zero(b)) { \
        cact_raise(cact, cact_make_error(cact, "Division by zero", CACT_NULL_VAL)); \
    } \
	if (cact_is_flonum(a) || cact_is_flonum(b)) { \
		struct cact_val fa, fb; \
		fa = cact_to_inexact(cact, a); \
		fb = cact_to_inexact(cact, b); \
 \
		return CACT_FLO_VAL(fa.as.flonum op fb.as.flonum); \
	} else { \
		return CACT_FIX_VAL(a.as.fixnum op b.as.fixnum); \
	} \
}

CACT_DEFINE_DIVISION_OP(cact_divide, /)

#undef CACT_DEFINE_DIVISION_OP

struct cact_val
cact_builtin_plus(struct cactus *cact, struct cact_val args)
{
    struct cact_val sum = CACT_FIX_VAL(0);
    CACT_LIST_FOR_EACH_ITEM(cact, addend, args) {
        sum = cact_add(cact, sum, addend);
    }
    return sum;
}

struct cact_val
cact_builtin_times(struct cactus *cact, struct cact_val args)
{
    struct cact_val product = CACT_FIX_VAL(1);
    CACT_LIST_FOR_EACH_ITEM(cact, factor, args) {
        product = cact_multiply(cact, product, factor);
    }
    return product;
}

struct cact_val
cact_builtin_minus(struct cactus *cact, struct cact_val x)
{
    struct cact_val initial = cact_car(cact, x);
    if (cact_is_null(cact_cdr(cact, x))) {
        return cact_subtract(cact, CACT_FIX_VAL(0), initial);
    } else {
        struct cact_val rest = cact_cdr(cact, x);
        CACT_LIST_FOR_EACH_ITEM(cact, subtrahend, rest) {
            initial = cact_subtract(cact, initial, subtrahend);
        }
        return initial;
    }
}

struct cact_val
cact_builtin_divide(struct cactus *cact, struct cact_val x)
{
    struct cact_val initial = cact_car(cact, x);
    if (cact_is_null(cact_cdr(cact, x))) {
        return cact_divide(cact, CACT_FIX_VAL(1), initial);
    } else {
        struct cact_val rest = cact_cdr(cact, x);
        CACT_LIST_FOR_EACH_ITEM(cact, divisor, rest) {
            initial = cact_divide(cact, initial, divisor);
        }
        return initial;
    }
}
