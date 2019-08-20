/* file minunit_example.c */

#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../builtin.h"
#include "minunit.h"

int tests_run = 0;

static char * readsexp_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;

	// No string
	status = readsexp(&string, &x);
	mu_assert("readsexp with blank string did not return other error", status == READSEXP_OTHER_ERROR);

    // Blank string
	string = "";
	status = readsexp(&string, &x);
	mu_assert("readsexp with blank string did not return EOF", status == READSEXP_END_OF_FILE);

    // Null
    string = "()";
    status = readsexp(&string, &x);
	mu_assert("readsexp not ok when reading null", status == READSEXP_OK);
	mu_assert("readsexp did not read () as null", ! x);

	// Identifier
    string = "identifier";
    status = readsexp(&string, &x);
	mu_assert("readsexp not ok when reading identifier", status == READSEXP_OK);
	mu_assert("readsexp did not read an identifier", x && is_sym(x));

	// Integer
    string = "1234";
    status = readsexp(&string, &x);
	mu_assert("readsexp not ok when reading integer", status == READSEXP_OK);
	mu_assert("readsexp did not read an integer", x && is_int(x));

	// String 
    string = "\"identifier\"";
    status = readsexp(&string, &x);
	mu_assert("readsexp not ok when reading string", status == READSEXP_OK);
	mu_assert("readsexp did not read a string", x && is_str(x));

	// List
    string = "(a)";
    status = readsexp(&string, &x);
	mu_assert("readsexp not ok when reading list", status == READSEXP_OK);
	mu_assert("readsexp did not read a list", x && is_pair(x));

	mu_assert("readsexp did not read first item in list", car(x));
	mu_assert("readsexp did not read second item in list", ! cdr(x));

	return 0;
}

static char * all_tests() {
	mu_run_test(readsexp_test);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
