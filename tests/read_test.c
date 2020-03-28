#include <stdio.h>
#include "minunit.h"

#include "cactus/core.h"
#include "cactus/read.h"

#include "cactus/val.h"
#include "cactus/bool.h"
#include "cactus/sym.h"
#include "cactus/pair.h"
#include "cactus/num.h"
#include "cactus/str.h"

#include "cactus/builtin.h"

struct cactus cact;

static char *cact_read_null_test() {
	int status = CACT_READ_OK;
	struct cact_val x = CACT_NULL_VAL;
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
	struct cact_val x = CACT_NULL_VAL;
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
	struct cact_val x = CACT_NULL_VAL;
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
	struct cact_val x = CACT_NULL_VAL;
	char* string = NULL;
	cact_init(&cact);

	// Identifier
	string = "identifier";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading identifier", status == CACT_READ_OK);
	mu_assert("cact_read did not read an identifier", cact_is_symbol(x));

	return 0;
}

static char *cact_read_boolean_test() {
	int status = CACT_READ_OK;
	struct cact_val x = CACT_NULL_VAL;
	char* string = NULL;
	cact_init(&cact);

	/* True */
	string = "#t";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading boolean true", status == CACT_READ_OK);
	mu_assert("cact_read did not read a boolean", cact_is_bool(x) == true);
	mu_assert("cact_read did not read true", cact_to_bool(x, "cact_read_boolean_test") == true);

	string = "#f";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading boolean false", status == CACT_READ_OK);
	mu_assert("cact_read did not read a boolean", cact_is_bool(x) == true);
	mu_assert("cact_read did not read false", cact_to_bool(x, "cact_read_boolean_test") == false);

	return 0;
}


static char *cact_read_int_test() {
	int status = CACT_READ_OK;
	struct cact_val x = CACT_NULL_VAL;
	char* string = NULL;
	cact_init(&cact);

	// Integer
	string = "1234";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading integer", status == CACT_READ_OK);
	mu_assert("cact_read did not read an integer", cact_is_fixnum(x));

	return 0;
}

static char *cact_read_string_test() {
	int status = CACT_READ_OK;
	struct cact_val x = CACT_NULL_VAL;
	char* string = NULL;
	cact_init(&cact);

	// cact_string 
	string = "\"identifier\"";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading string", status == CACT_READ_OK);
	mu_assert("cact_read did not read a string", cact_is_string(x));
	mu_assert("cact_read included quotes", cact_to_string(x, "cact_read_string_test")->str[0] == 'i');

	return 0;
}

static char *cact_read_list_test() {
	int status = CACT_READ_OK;
	struct cact_val x = CACT_NULL_VAL;
	char* string = NULL;
	cact_init(&cact);

	// Null
	string = "()";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading null", status == CACT_READ_OK);
	mu_assert("cact_read did not read () as null", cact_is_null(x));

	// List
	string = "(a)";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading list with one item", status == CACT_READ_OK);
	mu_assert("cact_read did not read a list with one item", cact_is_pair(x));

	mu_assert("cact_read did not read first item in list", ! cact_is_null(cact_car(&cact, x)));
	mu_assert("cact_read did not read second item in list", cact_is_null(cact_cdr(&cact, x)));

	string = "(a b c d)";
	cact_lexer_init(&cact.lexer, string);
	status = cact_read(&cact, &x);
	mu_assert("cact_read not ok when reading list", status == CACT_READ_OK);
	mu_assert("cact_read did not read a list", cact_is_pair(x));

    CACT_LIST_FOR_EACH_PAIR(&cact, p, x) {
        mu_assert(
	        "cact_read misread a symbol", 
	        cact_is_symbol(cact_car(&cact, *p))
	    );
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
	struct cact_val x = CACT_NULL_VAL;
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

