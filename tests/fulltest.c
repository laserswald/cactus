/* file minunit_example.c */

#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../builtin.h"
#include "minunit.h"

int tests_run = 0;

static char *readsexp_null_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;

	struct lexer l;

	// No string
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp with blank string did not return other error", status == READSEXP_OTHER_ERROR);
	return 0;
}

static char *readsexp_blank_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Blank string
	string = "";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp with blank string did not return EOF", status == READSEXP_END_OF_FILE);

	return 0;
}

static char *readsexp_ident_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Identifier
	string = "identifier";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp not ok when reading identifier", status == READSEXP_OK);
	mu_assert("readsexp did not read an identifier", x && is_sym(x));

	return 0;
}

static char *readsexp_int_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Integer
	string = "1234";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp not ok when reading integer", status == READSEXP_OK);
	mu_assert("readsexp did not read an integer", x && is_int(x));

	return 0;
}

static char *readsexp_string_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// String 
	string = "\"identifier\"";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp not ok when reading string", status == READSEXP_OK);
	mu_assert("readsexp did not read a string", x && is_str(x));

	return 0;
}

static char *readsexp_list_test() {
	int status = READSEXP_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Null
	string = "()";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp not ok when reading null", status == READSEXP_OK);
	mu_assert("readsexp did not read () as null", ! x);

	// List
	string = "(a)";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp not ok when reading list", status == READSEXP_OK);
	mu_assert("readsexp did not read a list", x && is_pair(x));

	mu_assert("readsexp did not read first item in list", car(x));
	mu_assert("readsexp did not read second item in list", ! cdr(x));

	string = "(a b c d)";
	lexer_init(&l, string);
	status = readsexp(&l, &x);
	mu_assert("readsexp not ok when reading list", status == READSEXP_OK);
	mu_assert("readsexp did not read a list", x && is_pair(x));

        LIST_FOR_EACH(x, p) {
	        mu_assert("readsexp misread a symbol", car(p)->t == TYPE_SYMBOL);
        }

	string = "(define double (lambda (x) (+ x x)))";
	lexer_init(&l, string);
	// printtokstream(&l);

	status = readsexp(&l, &x);
	print_sexp(x);
	mu_assert("readsexp not ok when reading list", status == READSEXP_OK);
	return 0;
}

// static char * readsexp_quote_test() {
// 	int status = READSEXP_OK;
// 	Sexp *x = NULL;
// 	char* string = NULL;
// 	struct lexer l;

// 	return 0;
// }

static char * all_tests() {
	mu_run_test(readsexp_null_test);
	mu_run_test(readsexp_blank_test);
	mu_run_test(readsexp_ident_test);
	mu_run_test(readsexp_int_test);
	mu_run_test(readsexp_string_test);
	mu_run_test(readsexp_list_test);
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
