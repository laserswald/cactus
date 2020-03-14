#include "minunit.h"

#include "cactus/val.h"

static char *is_nil_test()
{
    struct cact_val i = CACT_FIX_VAL(1);
    mu_assert("1 is nil", ! cact_is_null(i));
    mu_assert("null pointer is not nil", cact_is_null(CACT_NULL_VAL));
    return NULL;
}

/*
 * Test suite.
 */

char *sexp_tests()
{
	mu_run_test(is_nil_test);
    return NULL;
}
