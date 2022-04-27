#ifndef CACT_READ_H
#define CACT_READ_H

typedef struct cact_value cact_value_t;
typedef struct cact_context cact_context_t;

typedef enum cact_read_status {
	CACT_READ_IN_PROGRESS,
	CACT_READ_OK,
	CACT_READ_END_OF_FILE,
	CACT_READ_UNMATCHED_CHAR,
	CACT_READ_OTHER_ERROR,
} cact_read_status_t;

typedef enum cact_token {
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
} cact_token_t;

/* Holds a human-readable point in a file. */
typedef struct cact_str_coords {
	int line;
	int col;
	int bytes;
} cact_str_coords_t;

void cact_str_coords_init(cact_str_coords_t *);

/* Adjust the coordinates given the next character */
void cact_str_coords_push(cact_str_coords_t *, int);

typedef struct cact_lexeme {
	cact_str_coords_t coords;
	cact_token_t t;
	char *st;
	size_t sz;
} cact_lexeme_t;

typedef struct cact_lexer {
	const char *st;
	char *cur;
	cact_str_coords_t coords;
	cact_lexeme_t buf;
} cact_lexer_t;

void cact_lexer_init(cact_lexer_t *l, const char* data);
void printtokstream(cact_lexer_t *l);
cact_read_status_t cact_read(cact_context_t *l, cact_value_t *s);

#endif // CACT_READ_H

