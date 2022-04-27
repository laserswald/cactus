#include <stdio.h>
#include "greatest.h"

#include "core.h"
#include "evaluator/eval.h"
#include "num.h"
#include "err.h"
#include "write.h"
#include "builtin.h"

static cact_context_t cact;

TEST
eval_selfeval_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "1");
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 1", cact_to_long(v, "eval_selfeval_test") == 1);

    PASS();
}

TEST
eval_quote_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "'abc");
    ASSERTm("expected value to be a symbol", cact_is_symbol(v));

    PASS();
}

TEST
eval_define_var_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "(define x 1) x");
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 1", cact_to_long(v, "eval_define_test") == 1);

    PASS();
}

TEST
eval_if_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "(if #t 1 2)");
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 1", cact_to_long(v, "eval_if_test") == 1);

    PASS();
}

TEST
eval_if_false_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "(if #f 1 2)");
    cact_display(v);
    ASSERTm("expected x to be int", cact_is_fixnum(v));
    ASSERTm("expected x to be 2", cact_to_long(v, "eval_if_test") == 2);
    PASS();
}

TEST
eval_setbang_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "(define x 2) (set! x 4) x");

    ASSERTm("expect x to be fixnum", cact_is_fixnum(v));
    ASSERTm("expect x to be 4", cact_to_long(v, "eval_setbang_test") == 4);

    PASS();
}

TEST
eval_apply_builtin_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "(cons 1 2)");
    ASSERTm("cons did not return pair as expected", cact_is_pair(v));
    ASSERTm("car was not 1", cact_to_long(cact_car(&cact, v), "eval_pairs_test") == 1);
    ASSERTm("cdr was not 2", cact_to_long(cact_cdr(&cact, v), "eval_pairs_test") == 2);
    PASS();
}

TEST
eval_apply_test()
{
    cact_value_t v;

    v = cact_eval_string(&cact, "(define square (lambda (x) (* x x))) (square 8)");
    ASSERTm("lambda did not return int as expected", cact_is_fixnum(v));
    ASSERTm("lambda did not return square of 8", cact_to_long(v, "eval_define_lambda_test") == 64);

    PASS();
}

SUITE(eval_tests)
{
    cact_init(&cact);

    cact_define_builtin(&cact, "*", cact_builtin_times);
    cact_define_builtin(&cact, "+", cact_builtin_plus);
    cact_define_builtin(&cact, "cons", cact_builtin_cons);

    RUN_TEST(eval_selfeval_test);
    RUN_TEST(eval_quote_test);
    RUN_TEST(eval_define_var_test);
    RUN_TEST(eval_if_test);
    RUN_TEST(eval_if_false_test);
    RUN_TEST(eval_setbang_test);
    RUN_TEST(eval_apply_builtin_test);
    RUN_TEST(eval_apply_test);

    cact_finish(&cact);
}
