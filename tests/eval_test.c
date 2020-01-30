#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../sym.h"
#include "../builtin.h"
#include "minunit.h"
#include "../utils.h"
#include "../core.h"

char *eval_lambda_test() {
	struct cactus cact;
	struct cact_val *v;

	cact_init(&cact);

	v = cact_eval_string(&cact, "(define (square x) (* x x)) (square 8)");
	mu_assert("lambda did not return int as expected", is_int(v));
	fprintf(stderr, "%d\n", v->i);
	mu_assert("lambda did not return square of 8", v->i == 64);

    free(v);
	cact_finish(&cact);

    return NULL;
}

char *eval_tests() {
	mu_run_test(eval_lambda_test);
	return NULL;
}
