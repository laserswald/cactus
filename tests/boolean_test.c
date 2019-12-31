#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../sym.h"
#include "../builtin.h"
#include "minunit.h"
#include "../utils.h"

static char*
not_test()
{
    struct {
	    cact_val *thing;
	    bool expected;
    } cases[] = {
        {NULL, false},
        {cact_make_boolean(true), false},
        {cact_make_integer(3), false},
        {cons(cact_make_integer(3), NULL), false},
        {cact_make_boolean(false), true},
        {cact_make_symbol("nil"), false},
    };

    size_t len_cases = LENGTH(cases);
    int i;
    for (i = 0; i < len_cases; i++) {
	    mu_assert("`not` failed", sexp_not(cases[i].thing)->b == cases[i].expected);
    }

	return 0;
}

static char*
is_boolean_test()
{
    struct {
	    cact_val *thing;
	    bool expected;
    } cases[] = {
        {NULL, false},
        {cact_make_boolean(false), true},
        {cact_make_boolean(true), true},
        {cact_make_integer(0), false},
    };

    size_t len_cases = LENGTH(cases);
    int i;
    for (i = 0; i < len_cases; i++) {
	    mu_assert("`is_bool` failed", is_bool(cases[i].thing) == cases[i].expected);
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
