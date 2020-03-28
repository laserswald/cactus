#include <stdio.h>
#include "minunit.h"

#include "cactus/core.h"
#include "cactus/read.h"
#include "cactus/val.h"
#include "cactus/pair.h"
#include "cactus/sym.h"
#include "cactus/bool.h"
#include "cactus/internal/utils.h"

static char*
not_test()
{
	struct cactus cact;
	cact_init(&cact);

    struct {
	    struct cact_val thing;
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
	    mu_assert("`not` failed", cact_to_bool(cact_bool_not(cases[i].thing), "not_test") == cases[i].expected);
    }

    cact_finish(&cact);

	return 0;
}

static char*
is_boolean_test()
{
    struct {
	    struct cact_val thing;
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
	    mu_assert("`is_bool` failed", cact_is_bool(cases[i].thing) == cases[i].expected);
    }

	return 0;
}

char*
boolean_tests()
{
	mu_run_test(not_test);
	mu_run_test(is_boolean_test);
	return 0;
}
