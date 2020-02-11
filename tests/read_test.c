#include <stdio.h>
#include "../read.h"
#include "../sexp.h"
#include "../builtin.h"
#include "minunit.h"

struct cactus cact;

static char *cact_read_null_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// No string
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read with blank string did not return other error", status == CACT_READ_OTHER_ERROR);

	cact_finish(&cact);
	return 0;
}

static char *cact_read_blank_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// Blank string
	string = "";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read with blank string did not return EOF", status == CACT_READ_END_OF_FILE);

	return 0;
}

static char *cact_read_whitespace_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// Blank string
	string = "\n \t";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read with only whitespace did not return EOF", status == CACT_READ_END_OF_FILE);

	return 0;
}

static char *cact_read_ident_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// Identifier
	string = "identifier";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading identifier", status == CACT_READ_OK);
	mu_assert("cact_read did not read an identifier", x && is_sym(x));

	return 0;
}

static char *cact_read_boolean_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	/* True */
	string = "#t";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading boolean true", status == CACT_READ_OK);
	mu_assert("cact_read did not read a boolean", is_bool(x) == true);
	mu_assert("cact_read did not read true", x->b == true);

	string = "#f";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading boolean false", status == CACT_READ_OK);
	mu_assert("cact_read did not read a boolean", is_bool(x) == true);
	mu_assert("cact_read did not read false", x->b == false);

	return 0;
}


static char *cact_read_int_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// Integer
	string = "1234";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading integer", status == CACT_READ_OK);
	mu_assert("cact_read did not read an integer", x && is_int(x));

	return 0;
}

static char *cact_read_string_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// cact_string 
	string = "\"identifier\"";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading string", status == CACT_READ_OK);
	mu_assert("cact_read did not read a string", x && is_str(x));
	mu_assert("cact_read included quotes", x->s.str[0] == 'i');

	return 0;
}

static char *cact_read_list_test() {
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

	// Null
	string = "()";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading null", status == CACT_READ_OK);
	mu_assert("cact_read did not read () as null", ! x);

	// List
	string = "(a)";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading list with one item", status == CACT_READ_OK);
	mu_assert("cact_read did not read a list with one item", x && is_pair(x));

	mu_assert("cact_read did not read first item in list", car(x));
	mu_assert("cact_read did not read second item in list", ! cdr(x));

	string = "(a b c d)";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading list", status == CACT_READ_OK);
	mu_assert("cact_read did not read a list", x && is_pair(x));

    LIST_FOR_EACH(x, p) {
        mu_assert("cact_read misread a symbol", car(p)->t == CACT_TYPE_SYMBOL);
    }

	string = "(define double (lambda (x) (+ x x)))";
	cact_lexer_init(&cact.lexer, string);
	// printtokstream(&l);

	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading list", status == CACT_READ_OK);
	return 0;
}

static char*
cact_read_quote_test()
{
	int status = CACT_READ_OK;
	cact_val *x = NULL;
	char* string = NULL;
	cact_init(&cact);

    string = "'a";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading quoted symbol", status == CACT_READ_OK);

	return 0;
}

char *read_tests() {
	mu_run_test(cact_read_null_test);
	mu_run_test(cact_read_blank_test);
	mu_run_test(cact_read_whitespace_test);
	mu_run_test(cact_read_ident_test);
	mu_run_test(cact_read_int_test);
	mu_run_test(cact_read_boolean_test);
	mu_run_test(cact_read_string_test);
	mu_run_test(cact_read_list_test);
	mu_run_test(cact_read_quote_test);
	return 0;
}

