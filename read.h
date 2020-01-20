#ifndef CACT_READ_H
#define CACT_READ_H

#include "sexp.h"

struct cact_val;

enum cact_read_status {
    CACT_READ_OK,
    CACT_READ_END_OF_FILE,
    CACT_READ_UNMATCHED_CHAR,
    CACT_READ_OTHER_ERROR,
};

enum cact_token {
    CACT_TOKEN_ERROR = -2,
    CACT_TOKEN_END = -1,
    CACT_TOKEN_NOTHING = 0,
    CACT_TOKEN_COMMENT,
    CACT_TOKEN_WHITESPACE,
    CACT_TOKEN_IDENTIFIER,
    CACT_TOKEN_BOOLEAN,
    CACT_TOKEN_CHARACTER,
    CACT_TOKEN_INTEGER,
    CACT_TOKEN_FLOAT,
    CACT_TOKEN_STRING,
    CACT_TOKEN_OPEN_PAREN,
    CACT_TOKEN_CLOSE_PAREN,
    CACT_TOKEN_VECTOR_OPEN,
    CACT_TOKEN_BYTESTRING_OPEN,
    CACT_TOKEN_SINGLE_QUOTE,
    CACT_TOKEN_BACKQUOTE,
    CACT_TOKEN_COMMA,
    CACT_TOKEN_COMMA_SPLICE,
    CACT_TOKEN_DOT,
    CACT_NUM_TOKENS,
};

struct cact_lexeme {
    enum cact_token t;
    char *st;
    size_t sz;
    int lno;
};

struct cact_lexer {
    const char *st;
    char *cur;
    int lno;
    struct cact_lexeme buf;
};

void cact_lexer_init(struct cact_lexer *l, const char* data);
void printtokstream(struct cact_lexer *l);
enum cact_read_status cact_read(struct cact_lexer *l, struct cact_val **s);

#endif // CACT_READ_H

