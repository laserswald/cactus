#include <stdio.h>
#include "minunit.h"

#include "cactus/core.h"
#include "cactus/eval.h"
#include "cactus/num.h"
#include "cactus/err.h"
#include "cactus/write.h"
#include "cactus/builtin.h"

static struct cactus cact;

char *
eval_define_test()
{
	struct cact_val v;

	v = cact_eval_string(&cact, "(define x 1) x");
	mu_assert("expected x to be int", cact_is_fixnum(v));
	mu_assert("expected x to be 1", cact_to_long(v, "eval_define_test") == 1);

	v = cact_eval_string(&cact, "(define (square x) (* x x)) (square 8)");
	mu_assert("lambda did not return int as expected", cact_is_fixnum(v));
	mu_assert("lambda did not return square of 8", cact_to_long(v, "eval_lambda_test") == 64);

    return NULL;
}

char *
eval_if_test()
{
	struct cact_val v;

	v = cact_eval_string(&cact, "(if #t 1 2)");
	mu_assert("expected x to be int", cact_is_fixnum(v));
	mu_assert("expected x to be 1", cact_to_long(v, "eval_if_test") == 1);

	v = cact_eval_string(&cact, "(if #f 1 2)");
	mu_assert("expected x to be int", cact_is_fixnum(v));
	mu_assert("expected x to be 2", cact_to_long(v, "eval_if_test") == 2);

    return NULL;
}


char *
eval_pairs_test() 
{
	struct cact_val v;

	v = cact_eval_string(&cact, "(cons 1 2)");
	mu_assert("lambda did not return pair as expected", cact_is_pair(v));
	mu_assert("car was not 1", cact_to_long(cact_car(&cact, v), "eval_pairs_test") == 1);
	mu_assert("cdr was not 2", cact_to_long(cact_cdr(&cact, v), "eval_pairs_test") == 2);

    return NULL;
}

char *
eval_tests() 
{
	cact_init(&cact);

	cact_define_builtin(&cact, "*", cact_builtin_times);
	cact_define_builtin(&cact, "cons", cact_builtin_cons);

	mu_run_test(eval_define_test);
	mu_run_test(eval_if_test);
	mu_run_test(eval_pairs_test);

	cact_finish(&cact);

	return NULL;
}
