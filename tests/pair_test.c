#include "minunit.h"

#include "cactus/core.h"
#include "cactus/pair.h"
#include "cactus/num.h"
#include "cactus/write.h"

static struct cactus cact;

static char *car_cdr_cons_test()
{
    int li = rand();
    struct cact_val l = CACT_FIX_VAL(li);
    int ri = rand();
    struct cact_val r = CACT_FIX_VAL(ri);
    struct cact_val pair = cact_cons(&cact, l, r);

    mu_assert("cons did not create a new pair", cact_is_pair(pair));
    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "car_cdr_cons_test") == li);
    mu_assert("cdr of pair was not generated number", cact_to_long(cact_cdr(&cact, pair), "car_cdr_cons_test") == ri);

    return NULL;
}

static char *append_test()
{
    struct cact_val pair = CACT_NULL_VAL;

    // 0 case
    int first = rand();
    struct cact_val fst = CACT_FIX_VAL(first);

    pair = cact_append(&cact, pair, fst);

    cact_preserve(&cact, pair);
    mu_assert("cact_append did not create a new pair", cact_is_pair(pair));
    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "append_test") == first);
    mu_assert("cdr of pair was not nil", cact_is_null(cact_cdr(&cact, pair)));

    // 1 case
    int second = rand();
    struct cact_val snd = CACT_FIX_VAL(second);

    pair = cact_append(&cact, pair, snd);

    mu_assert("cact_append did not create a new pair", cact_is_pair(pair));
    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "append_test") == first);

    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, cact_cdr(&cact, pair)), "append_test") == second);
    mu_assert("cdr of pair was not nil", cact_is_null(cact_cdr(&cact, cact_cdr(&cact, pair))));

    // N case
    int third = rand();
    struct cact_val thd = CACT_FIX_VAL(third);

    pair = cact_append(&cact, pair, thd);

    mu_assert("cact_append did not create a new pair", cact_is_pair(pair));
    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "append_test") == first);
    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, cact_cdr(&cact, pair)), "append_test") == second);
    mu_assert("car of pair was not generated number", cact_to_long(cact_car(&cact, cact_cdr(&cact, cact_cdr(&cact, pair))), "append_test") == third);
    mu_assert("cdr of pair was not nil", cact_is_null(cact_cdr(&cact, cact_cdr(&cact, cact_cdr(&cact, pair)))));
    cact_unpreserve(&cact, pair);

    fprintf(stderr, "success!\n");

    return NULL;
}

static char *assoc_list_test()
{
    return NULL;
}

char *pair_tests()
{
	cact_init(&cact);
    mu_run_test(car_cdr_cons_test);
    mu_run_test(append_test);
    mu_run_test(assoc_list_test);
	cact_finish(&cact);
    return NULL;
}

