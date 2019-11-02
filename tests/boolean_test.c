#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../builtin.h"
#include "minunit.h"

static char*
not_test()
{
	Sexp *truth = make_boolean(true);
	Sexp *falsity = sexp_not(truth);
    mu_assert("not did not invert truth", falsity->b == false);
	return 0;
}

static char*
boolean_tests()
{
	mu_run_test(not_test);
	return 0;
}
