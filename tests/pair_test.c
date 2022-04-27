#include "greatest.h"

#include "core.h"
#include "pair.h"
#include "num.h"
#include "write.h"

static cact_context_t cact;

TEST car_cdr_cons_test()
{
    int li = rand();
    cact_value_t l = CACT_FIX_VAL(li);
    int ri = rand();
    cact_value_t r = CACT_FIX_VAL(ri);
    cact_value_t pair = cact_cons(&cact, l, r);

    ASSERTm("cons did not create a new pair", cact_is_pair(pair));
    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "car_cdr_cons_test") == li);
    ASSERTm("cdr of pair was not generated number", cact_to_long(cact_cdr(&cact, pair), "car_cdr_cons_test") == ri);

    PASS();
}

TEST append_test()
{
    cact_value_t pair = CACT_NULL_VAL;

    // 0 case
    int first = rand();
    cact_value_t fst = CACT_FIX_VAL(first);

    pair = cact_append(&cact, pair, fst);

    cact_preserve(&cact, pair);
    ASSERTm("cact_append did not create a new pair", cact_is_pair(pair));
    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "append_test") == first);
    ASSERTm("cdr of pair was not nil", cact_is_null(cact_cdr(&cact, pair)));

    // 1 case
    int second = rand();
    cact_value_t snd = CACT_FIX_VAL(second);

    pair = cact_append(&cact, pair, snd);

    ASSERTm("cact_append did not create a new pair", cact_is_pair(pair));
    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "append_test") == first);

    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, cact_cdr(&cact, pair)), "append_test") == second);
    ASSERTm("cdr of pair was not nil", cact_is_null(cact_cdr(&cact, cact_cdr(&cact, pair))));

    // N case
    int third = rand();
    cact_value_t thd = CACT_FIX_VAL(third);

    pair = cact_append(&cact, pair, thd);

    ASSERTm("cact_append did not create a new pair", cact_is_pair(pair));
    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, pair), "append_test") == first);
    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, cact_cdr(&cact, pair)), "append_test") == second);
    ASSERTm("car of pair was not generated number", cact_to_long(cact_car(&cact, cact_cdr(&cact, cact_cdr(&cact, pair))), "append_test") == third);
    ASSERTm("cdr of pair was not nil", cact_is_null(cact_cdr(&cact, cact_cdr(&cact, cact_cdr(&cact, pair)))));
    cact_unpreserve(&cact, pair);

    fprintf(stderr, "success!\n");

    PASS();
}

TEST assoc_list_test()
{
    PASS();
}

SUITE(pair_tests)
{
    cact_init(&cact);
    RUN_TEST(car_cdr_cons_test);
    RUN_TEST(append_test);
    RUN_TEST(assoc_list_test);
    cact_finish(&cact);
}

