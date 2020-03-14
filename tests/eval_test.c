#include <stdio.h>
#include "minunit.h"

#include "cactus/core.h"
#include "cactus/eval.h"
#include "cactus/num.h"

char *eval_lambda_test() {
	struct cactus cact;
	struct cact_val v;

	cact_init(&cact);

	v = cact_eval_string(&cact, "(define (square x) (* x x)) (square 8)");
	mu_assert("lambda did not return int as expected", cact_is_fixnum(v));
	fprintf(stderr, "%d\n", cact_to_long(v, "eval_lambda_test"));
	mu_assert("lambda did not return square of 8", cact_to_long(v, "eval_lambda_test") == 64);

	cact_finish(&cact);

    return NULL;
}

char *eval_tests() {
	mu_run_test(eval_lambda_test);
	return NULL;
}
