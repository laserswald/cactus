#include <stdio.h>
#include "greatest.h"

#include "core.h"
#include "read.h"
#include "val.h"
#include "pair.h"
#include "sym.h"
#include "bool.h"
#include "internal/utils.h"

SUITE(boolean_tests);

TEST not_test()
{
    cact_context_t cact;
    cact_init(&cact);

    struct {
        cact_value_t thing;
        bool expected;
    } cases[] = {
        {CACT_NULL_VAL, false},
        {CACT_BOOL_VAL(true), false},
        {CACT_FIX_VAL(3), false},
        {cact_cons(&cact, CACT_FIX_VAL(3), CACT_NULL_VAL), false},
        {CACT_BOOL_VAL(false), true},
        {cact_make_symbol(&cact, "nil"), false},
    };

    size_t len_cases = LENGTH(cases);
    int i;
    for (i = 0; i < len_cases; i++) {
        ASSERT_EQm("`not` failed", cases[i].expected, cact_to_bool(cact_bool_not(cases[i].thing), "not_test"));
    }

    cact_finish(&cact);

    return 0;
}

TEST is_boolean_test()
{
    struct {
        cact_value_t thing;
        bool expected;
    } cases[] = {
        {CACT_NULL_VAL, false},
        {CACT_BOOL_VAL(false), true},
        {CACT_BOOL_VAL(true), true},
        {CACT_FIX_VAL(0), false},
    };

    size_t len_cases = LENGTH(cases);
    int i;
    for (i = 0; i < len_cases; i++) {
        ASSERT_EQm("`is_bool` failed", cases[i].expected, cact_is_bool(cases[i].thing));
    }

    return 0;
}

SUITE(boolean_tests)
{
    RUN_TEST(not_test);
    RUN_TEST(is_boolean_test);
}

