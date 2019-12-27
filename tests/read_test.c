#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../builtin.h"
#include "minunit.h"

static char *cact_read_null_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;

	struct lexer l;

	// No string
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read with blank string did not return other error", status == CACT_READ_OTHER_ERROR);
	return 0;
}

static char *cact_read_blank_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Blank string
	string = "";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read with blank string did not return EOF", status == CACT_READ_END_OF_FILE);

	return 0;
}

static char *cact_read_ident_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Identifier
	string = "identifier";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading identifier", status == CACT_READ_OK);
	mu_assert("cact_read did not read an identifier", x && is_sym(x));

	return 0;
}

static char *cact_read_boolean_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	/* True */
	string = "#t";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading boolean true", status == CACT_READ_OK);
	mu_assert("cact_read did not read a boolean", is_bool(x) == true);
	mu_assert("cact_read did not read true", x->b == true);

	string = "#f";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading boolean false", status == CACT_READ_OK);
	mu_assert("cact_read did not read a boolean", is_bool(x) == true);
	mu_assert("cact_read did not read false", x->b == false);

	return 0;
}


static char *cact_read_int_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Integer
	string = "1234";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading integer", status == CACT_READ_OK);
	mu_assert("cact_read did not read an integer", x && is_int(x));

	return 0;
}

static char *cact_read_string_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// String 
	string = "\"identifier\"";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading string", status == CACT_READ_OK);
	mu_assert("cact_read did not read a string", x && is_str(x));
	mu_assert("cact_read included quotes", x->s.str[0] == 'i');

	return 0;
}

static char *cact_read_list_test() {
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

	// Null
	string = "()";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading null", status == CACT_READ_OK);
	mu_assert("cact_read did not read () as null", ! x);

	// List
	string = "(a)";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading list", status == CACT_READ_OK);
	mu_assert("cact_read did not read a list", x && is_pair(x));

	mu_assert("cact_read did not read first item in list", car(x));
	mu_assert("cact_read did not read second item in list", ! cdr(x));

	string = "(a b c d)";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading list", status == CACT_READ_OK);
	mu_assert("cact_read did not read a list", x && is_pair(x));

    LIST_FOR_EACH(x, p) {
        mu_assert("cact_read misread a symbol", car(p)->t == TYPE_SYMBOL);
    }

	string = "(define double (lambda (x) (+ x x)))";
	lexer_init(&l, string);
	// printtokstream(&l);

	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading list", status == CACT_READ_OK);
	return 0;
}

static char*
cact_read_quote_test()
{
	int status = CACT_READ_OK;
	Sexp *x = NULL;
	char* string = NULL;
	struct lexer l;

    string = "'a";
	lexer_init(&l, string);
	status = cact_read(&l, &x);
	mu_assert("cact_read not ok when reading quoted symbol", status == CACT_READ_OK);

	return 0;
}

char *read_tests() {
	mu_run_test(cact_read_null_test);
	mu_run_test(cact_read_blank_test);
	mu_run_test(cact_read_ident_test);
	mu_run_test(cact_read_int_test);
	mu_run_test(cact_read_boolean_test);
	mu_run_test(cact_read_string_test);
	mu_run_test(cact_read_list_test);
	mu_run_test(cact_read_quote_test);
	return 0;
}

