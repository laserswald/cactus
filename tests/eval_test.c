#include <stdio.h>
#include "greatest.h"

#include "cactus/core.h"
#include "cactus/eval.h"
#include "cactus/num.h"
#include "cactus/err.h"
#include "cactus/write.h"
#include "cactus/builtin.h"

static struct cactus cact;

TEST
eval_define_test()
{
    struct cact_val v;

    v = cact_eval_string(&cact, "(define x 1) x");
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 1", cact_to_long(v, "eval_define_test") == 1);

    v = cact_eval_string(&cact, "(define (square x) (* x x)) (square 8)");
    ASSERTm("lambda did not return int as expected", cact_is_fixnum(v));
    ASSERTm("lambda did not return square of 8", cact_to_long(v, "eval_lambda_test") == 64);
    PASS();
}

TEST
eval_if_test()
{
    struct cact_val v;

    v = cact_eval_string(&cact, "(if #t 1 2)");
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 1", cact_to_long(v, "eval_if_test") == 1);

    v = cact_eval_string(&cact, "(if #f 1 2)");
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 2", cact_to_long(v, "eval_if_test") == 2);
    PASS();
}

TEST
eval_pairs_test()
{
    struct cact_val v;

    v = cact_eval_string(&cact, "(cons 1 2)");
    ASSERTm("lambda did not return pair as expected", cact_is_pair(v));
    ASSERTm("car was not 1", cact_to_long(cact_car(&cact, v), "eval_pairs_test") == 1);
    ASSERTm("cdr was not 2", cact_to_long(cact_cdr(&cact, v), "eval_pairs_test") == 2);
    PASS();
}

TEST
eval_setbang_test()
{
    struct cact_val v;

    v = cact_eval_string(&cact, "(define x 2) (set! x 4) x");
    ASSERTm("eval_setbang_test: expect x to be fixnum", cact_is_fixnum(v));
    ASSERTm("eval_setbang_test: expect x to be 4", cact_to_long(v, "eval_setbang_test") == 4);

    PASS();
}

SUITE(eval_tests)
{
    cact_init(&cact);

    cact_define_builtin(&cact, "*", cact_builtin_times);
    cact_define_builtin(&cact, "cons", cact_builtin_cons);

    RUN_TEST(eval_define_test);
    RUN_TEST(eval_if_test);
    RUN_TEST(eval_pairs_test);
    RUN_TEST(eval_setbang_test);

    cact_finish(&cact);
}
