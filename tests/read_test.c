#include <stdio.h>
#include "greatest.h"

#include "core.h"
#include "read.h"

#include "val.h"
#include "bool.h"
#include "sym.h"
#include "pair.h"
#include "num.h"
#include "str.h"

#include "builtin.h"

cact_context_t cact;

TEST cact_read_null_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;
    cact_init(&cact);

    // No string
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read with blank string did not return other error", status == CACT_READ_OTHER_ERROR);

    cact_finish(&cact);
    PASS();
}

TEST cact_read_blank_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;
    cact_init(&cact);

    // Blank string
    string = "";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read with blank string did not return EOF", status == CACT_READ_END_OF_FILE);

    PASS();
}

TEST cact_read_whitespace_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;
    cact_init(&cact);

    // Blank string
    string = "\n \t";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read with only whitespace did not return EOF", status == CACT_READ_END_OF_FILE);

    PASS();
}

TEST cact_read_ident_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    // Identifier
    string = "identifier";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading identifier", status == CACT_READ_OK);
    ASSERTm("cact_read did not read an identifier", cact_is_symbol(x));

    // Identifier with dash
    string = "ident-ifier";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading identifier", status == CACT_READ_OK);
    ASSERTm("cact_read did not read an identifier", cact_is_symbol(x));

    cact_value_t real_sym = cact_make_symbol(&cact, string);

#ifdef CACT_DEBUG
    cact_display(real_sym);
    puts("");
    cact_display(x);
    puts("");
#endif

    ASSERTm(
	    "cact_read did not read the correct identifier", 
	    cact_value_eq(x, real_sym)
    );


    PASS();
}

TEST cact_read_boolean_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    /* True */
    string = "#t";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading boolean true", status == CACT_READ_OK);
    ASSERTm("cact_read did not read a boolean", cact_is_bool(x) == true);
    ASSERTm("cact_read did not read true", cact_to_bool(x, "cact_read_boolean_test") == true);

    string = "#f";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading boolean false", status == CACT_READ_OK);
    ASSERTm("cact_read did not read a boolean", cact_is_bool(x) == true);
    ASSERTm("cact_read did not read false", cact_to_bool(x, "cact_read_boolean_test") == false);

    PASS();
}


TEST cact_read_int_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    // unsigned
    string = "1234";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading integer", status == CACT_READ_OK);
    ASSERTm("cact_read did not read an integer", cact_is_fixnum(x));

    // signed
    string = "-1234";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading integer", status == CACT_READ_OK);
    ASSERTm("cact_read did not read an integer", cact_is_fixnum(x));

    PASS();
}

TEST cact_read_float_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    // unsigned
    string = "12.34";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading unsigned float", status == CACT_READ_OK);
    ASSERTm("cact_read did not read an unsigned float", cact_is_flonum(x));

    // signed
    string = "-12.34";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading signed float", status == CACT_READ_OK);
    ASSERTm("cact_read did not read a signed float", cact_is_flonum(x));

    PASS();
}

TEST cact_read_string_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    // cact_string
    string = "\"identifier\"";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading string", status == CACT_READ_OK);
    ASSERTm("cact_read did not read a string", cact_is_string(x));
    ASSERTm("cact_read included quotes", cact_to_string(x, "cact_read_string_test")->str[0] == 'i');

    PASS();
}

TEST cact_read_list_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    // Null
    string = "()";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading null", status == CACT_READ_OK);
    ASSERTm("cact_read did not read () as null", cact_is_null(x));

    // List
    string = "(a)";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    cact_preserve(&cact, x);
    ASSERTm("cact_read not ok when reading list with one item", status == CACT_READ_OK);
    ASSERTm("cact_read did not read a list with one item", cact_is_pair(x));

    ASSERTm("cact_read did not read first item in list", ! cact_is_null(cact_car(&cact, x)));
    ASSERTm("cact_read did not read second item in list", cact_is_null(cact_cdr(&cact, x)));

    string = "(d c b a)";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading list", status == CACT_READ_OK);
    ASSERTm("cact_read did not read a list", cact_is_pair(x));

    CACT_LIST_FOR_EACH_PAIR(&cact, p, x) {
        ASSERTm(
            "cact_read misread a symbol",
            cact_is_symbol(cact_car(&cact, *p))
        );
    }

    string = "(define double (lambda (x) (+ x x)))";
    cact_lexer_init(&cact.lexer, string);
    // printtokstream(&l);

    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading list", status == CACT_READ_OK);
    PASS();
}

TEST cact_read_quote_test()
{
    int status = CACT_READ_OK;
    cact_value_t x = CACT_NULL_VAL;
    char* string = NULL;

    string = "'a";
    cact_lexer_init(&cact.lexer, string);
    status = cact_read(&cact, &x);
    ASSERTm("cact_read not ok when reading quoted symbol", status == CACT_READ_OK);

    PASS();
}

SUITE(read_tests)
{
    cact_init(&cact);

    RUN_TEST(cact_read_null_test);
    RUN_TEST(cact_read_blank_test);
    RUN_TEST(cact_read_whitespace_test);
    RUN_TEST(cact_read_ident_test);
    RUN_TEST(cact_read_int_test);
    RUN_TEST(cact_read_float_test);
    RUN_TEST(cact_read_boolean_test);
    RUN_TEST(cact_read_string_test);
    RUN_TEST(cact_read_list_test);
    RUN_TEST(cact_read_quote_test);

    cact_finish(&cact);
}

