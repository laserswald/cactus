#include "greatest.h"

#include "val.h"

TEST is_nil_test()
{
    struct cact_val i = CACT_FIX_VAL(1);
    ASSERTm("1 is nil", ! cact_is_null(i));
    ASSERTm("null pointer is not nil", cact_is_null(CACT_NULL_VAL));
    PASS();
}

/*
 * Test suite.
 */
SUITE(sexp_tests)
{
    RUN_TEST(is_nil_test);
}
