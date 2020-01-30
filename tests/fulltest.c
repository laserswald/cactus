/* file minunit_example.c */

#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../builtin.h"
#include "minunit.h"

int tests_run = 0;

char *read_tests();
char *sexp_tests();
char *boolean_tests();
char *eval_tests();

static char *all_tests() {
	mu_run_test(read_tests);
	mu_run_test(sexp_tests);
	mu_run_test(boolean_tests);
	mu_run_test(eval_tests);
	return 0;
}

int main(int argc, char **argv) {
	char *result = all_tests();
	if (result != 0) {
		printf("FAILED: %s\n", result);
	}
	else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
