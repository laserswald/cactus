#include "cactus/num.h"

bool
cact_is_number(struct cact_val x)
{
    return cact_is_fixnum(x) || cact_is_flonum(x);
}

bool
cact_is_zero(struct cact_val x)
{
	if (cact_is_fixnum(x)) {
		return 0 == x.as.fixnum;
	} else if (cact_is_flonum(x)) {
		return 0.0 == x.as.flonum;
	} else {
		return false;
	}
}

bool
cact_is_positive(struct cact_val x)
{
	if (cact_is_fixnum(x)) {
		return 0 < x.as.fixnum;
	} else if (cact_is_flonum(x)) {
		return 0.0 < x.as.flonum;
	} else {
		return false;
	}
}

bool
cact_is_negative(struct cact_val x)
{
	if (cact_is_fixnum(x)) {
		return 0 > x.as.fixnum;
	} else if (cact_is_flonum(x)) {
		return 0.0 > x.as.flonum;
	} else {
		return false;
	}
}

#define DEFINE_NUMERIC_COMPARISON(name, op) \
bool \
name(struct cact_val a, struct cact_val b) \
{ \
	if (! cact_is_number(a) || ! cact_is_number(b)) { \
		fprintf(stderr, "Cannot compare non-numbers with numeric comparators."); \
		abort(); \
	} \
 \
	if (cact_is_fixnum(a) && cact_is_fixnum(b)) { \
		return a.as.fixnum op b.as.fixnum; \
	} else { \
		double ad = (cact_is_fixnum(a)) ? (double) a.as.fixnum : a.as.flonum; \
		double bd = (cact_is_fixnum(b)) ? (double) b.as.fixnum : b.as.flonum; \
		return ad op bd; \
	} \
}

DEFINE_NUMERIC_COMPARISON(cact_number_less, <)
DEFINE_NUMERIC_COMPARISON(cact_number_less_or_equal, <=)
DEFINE_NUMERIC_COMPARISON(cact_number_greater, >)
DEFINE_NUMERIC_COMPARISON(cact_number_greater_or_equal, >=)
DEFINE_NUMERIC_COMPARISON(cact_number_equal, ==)

#undef DEFINE_NUMERIC_COMPARISON

