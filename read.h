#ifndef read_h_INCLUDED
#define read_h_INCLUDED

#include "sexp.h"

enum read_status {
	READSEXP_OK,
	READSEXP_END_OF_FILE,
	READSEXP_OTHER_ERROR,
};

enum token {
	TOKEN_ERROR = -2,
	TOKEN_END = -1,
	TOKEN_NOTHING = 0,
	TOKEN_COMMENT,
	TOKEN_WHITESPACE,
	TOKEN_IDENTIFIER,
	TOKEN_BOOLEAN,
	TOKEN_CHARACTER,
	TOKEN_INTEGER,
	TOKEN_FLOAT,
	TOKEN_STRING,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_VECTOR_OPEN,
	TOKEN_BYTESTRING_OPEN,
	TOKEN_SINGLE_QUOTE,
	TOKEN_BACKQUOTE,
	TOKEN_COMMA,
	TOKEN_COMMA_SPLICE,
	TOKEN_DOT,
	LEN_TOKENS,
};

struct lexeme {
	enum token t;
	char *st;
	size_t sz;
	int lno;
};

struct lexer {
	const char *st;
	char *cur;
	int lno;
	struct lexeme buf;
};

struct lexer;

void lexer_init(struct lexer *l, const char* data);
void printtokstream(struct lexer *l);
int readsexp(struct lexer *l, Sexp **s);

#endif // read_h_INCLUDED

