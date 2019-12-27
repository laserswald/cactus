#include "../sexp.h"
#include "minunit.h"

static char *is_nil_test()
{
    cact_val *i = make_integer(1);
    mu_assert("1 is nil", ! is_nil(i));
    mu_assert("null pointer is not nil", is_nil(NULL));
    return NULL;
}

/*
 * Pair testing
 */

static char *car_cdr_cons_test()
{
    int li = rand();
    cact_val *l = make_integer(li);
    int ri = rand();
    cact_val *r = make_integer(ri);
    cact_val *pair = cons(l, r);

    mu_assert("cons did not create a new pair", is_pair(pair));
    mu_assert("car of pair was not generated number", car(pair)->i == li);
    mu_assert("cdr of pair was not generated number", cdr(pair)->i == ri);

    return NULL;
}

static char *append_test()
{
    cact_val *pair = NULL;

    // 0 case
    int first = rand();
    cact_val *fst = make_integer(first);

    pair = append(pair, fst);

    mu_assert("append did not create a new pair", is_pair(pair));
    mu_assert("car of pair was not generated number", car(pair)->i == first);
    mu_assert("cdr of pair was not nil", cdr(pair) == NULL);

    // 1 case
    int second = rand();
    cact_val *snd = make_integer(second);

    pair = append(pair, snd);

    mu_assert("append did not create a new pair", is_pair(pair));
    mu_assert("car of pair was not generated number", car(pair)->i == first);
    mu_assert("car of pair was not generated number", car(cdr(pair))->i == second);
    mu_assert("cdr of pair was not nil", cdr(cdr(pair)) == NULL);

    // N case
    int third = rand();
    cact_val *thd = make_integer(third);

    pair = append(pair, thd);

    mu_assert("append did not create a new pair", is_pair(pair));
    mu_assert("car of pair was not generated number", car(pair)->i == first);
    mu_assert("car of pair was not generated number", car(cdr(pair))->i == second);
    mu_assert("car of pair was not generated number", car(cdr(cdr(pair)))->i == third);
    mu_assert("cdr of pair was not nil", cdr(cdr(cdr(pair))) == NULL);

    return NULL;
}

static char *assoc_list_test()
{
    return NULL;
}

static char *pair_tests()
{
    mu_run_test(car_cdr_cons_test);
    mu_run_test(append_test);
    mu_run_test(assoc_list_test);
    return NULL;
}

/*
 * Test suite.
 */

char *sexp_tests()
{
	mu_run_test(is_nil_test);
    mu_run_test(pair_tests);
    return NULL;
}
