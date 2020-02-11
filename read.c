#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "read.h"
#include "debug.h"
#include "sexp.h"
#include "utils.h"
#include "sym.h"

void cact_str_coords_init(struct cact_str_coords *cds)
{
	memset(cds, 0, sizeof(struct cact_str_coords));
}

void cact_str_coords_push(struct cact_str_coords *cds, int c)
{
	cds->bytes++;
	if (c == '\n') {
		cds->col = 0;
		cds->line++;
	} else {
		cds->col++;
	}
}

/* Is this int allowable as an identifier character? */
int
is_ident(int i)
{
	return i != 0 && (isalpha(i) || strchr("!$%&*+-./:<=>?@^_~", i) != NULL);
}


/* Return 0 if the character is double quote, 1 otherwise. */
int 
notdblq(int c) 
{
	return c != '"';
}

/* Return 0 if the character is newline, 1 otherwise. */
int notnl(int c) {
	return c != '\n';
}

/* Initialize a lexer with a string to parse. */
void
cact_lexer_init(struct cact_lexer* l, const char* s)
{
	l->st = s;
	l->cur = (char*) s;
	cact_str_coords_init(&l->coords);
	l->buf.t = CACT_TOKEN_NOTHING;
}

static int
cact_lexer_getc(struct cact_lexer* l)
{
	if (! l->cur) {
		return -1;
	}

	if (*l->cur == 0) {
		return 0;
	}

	l->cur++;
	cact_str_coords_push(&l->coords, *l->cur);
	return *l->cur;
}

static int
cact_lexer_peekc(struct cact_lexer* l)
{
	if (! l->cur) {
		return -1;
	}
	return *l->cur;
}

/* Consume characters from the string until the function returns 0. */
static int
cact_lexer_charspan(struct cact_lexer* l, int (*fn)(int))
{
	int len = 0;
	while (cact_lexer_peekc(l) && fn(cact_lexer_peekc(l))) {
		cact_lexer_getc(l);
		len++;
	}
	return len;
}

/* Print a lexeme to stdout. */
void
printlex(struct cact_lexeme lx)
{
	switch (lx.t) {
	case CACT_TOKEN_ERROR:
		printf("error");
		break;
	case CACT_TOKEN_END:
		printf("end");
		break;
	case CACT_TOKEN_NOTHING:
		printf("nothing");
		break;
	case CACT_TOKEN_WHITESPACE:
		printf("whitespace");
		break;
	case CACT_TOKEN_IDENTIFIER:
		printf("identifier");
		break;
	case CACT_TOKEN_BOOLEAN:
		printf("boolean");
		break;
	case CACT_TOKEN_CHARACTER:
		printf("character");
		break;
	case CACT_TOKEN_INTEGER:
		printf("integer");
		break;
	case CACT_TOKEN_FLOAT:
		printf("float");
		break;
	case CACT_TOKEN_STRING:
		printf("string");
		break;
	case CACT_TOKEN_OPEN_PAREN:
		printf("open paren");
		break;
	case CACT_TOKEN_CLOSE_PAREN:
		printf("close paren");
		break;
	case CACT_TOKEN_SINGLE_QUOTE:
		printf("single quote");
		break;
	case CACT_TOKEN_COMMENT:
		printf("comment");
		break;
	default:
		break;
	}

	char* sym = strslice(lx.st, &lx.st[lx.sz]);

	printf(":\t'%s'\n", sym);
}

/* Return the next lexeme available from the lexer. */
struct cact_lexeme
nextlex(struct cact_lexer* l)
{
	struct cact_lexeme le = {
		.st = l->cur,
		.coords = l->coords
	};

	int c = cact_lexer_peekc(l);

	if (! c) {
		le.t = CACT_TOKEN_END;
	} else if (isspace(c)) {
		le.t = CACT_TOKEN_WHITESPACE;
		cact_lexer_charspan(l, isspace);
	} else if (is_ident(c)) {
		le.t = CACT_TOKEN_IDENTIFIER;
		cact_lexer_charspan(l, is_ident);
	} else if (isdigit(c)) {
		le.t = CACT_TOKEN_INTEGER;
		cact_lexer_charspan(l, isdigit);
	} else if (c == '(') {
		le.t = CACT_TOKEN_OPEN_PAREN;
		cact_lexer_getc(l);
	} else if (c == ')') {
		le.t = CACT_TOKEN_CLOSE_PAREN;
		cact_lexer_getc(l);
	} else if (c == '\'') {
		le.t = CACT_TOKEN_SINGLE_QUOTE;
		cact_lexer_getc(l);
	} else if (c == '"') {
		le.t = CACT_TOKEN_STRING;
		cact_lexer_getc(l);
		cact_lexer_charspan(l, notdblq);
		cact_lexer_getc(l);
	} else if (c == '#') { 
		/* boolean and character */
		c = cact_lexer_getc(l);
		if (c == 't' || c == 'f') {
			le.t = CACT_TOKEN_BOOLEAN;
			cact_lexer_charspan(l, isalpha);
		} else if (c == '\\') {
			cact_lexer_getc(l);
			le.t = CACT_TOKEN_CHARACTER;
			cact_lexer_charspan(l, isalpha);
		} else {
			le.t = CACT_TOKEN_ERROR;
		}
	} else if (c == ';') { 
		le.t = CACT_TOKEN_COMMENT;
		cact_lexer_charspan(l, notnl);
		cact_lexer_getc(l);
	} else if (c == -1) {
		le.t = CACT_TOKEN_ERROR;
	}

	le.sz = l->coords.bytes - le.coords.bytes;
	l->buf = le;

	return le;
}

/* Peek at the current lexeme. */
struct cact_lexeme
peeklex(struct cact_lexer *l)
{
	if (l->buf.t == CACT_TOKEN_NOTHING) {
		nextlex(l);
	}
	return l->buf;
}

/*
 * Peek at the current lexeme and return 1 if it is the specified
 * token, or 0 otherwise.
 */
int
peekistok(struct cact_lexer *l, enum cact_token t)
{
	struct cact_lexeme e = peeklex(l);
	return e.t == t;
}

/* If the current lexeme is of the specified token, consume it. */
int
expecttok(struct cact_lexer *l, enum cact_token t)
{
	int istok = peekistok(l, t);
	if (istok) {
		nextlex(l);
	}
	return istok;
}

/* Print the entire stream of tokens from the lexer. */
void
printtokstream(struct cact_lexer *l)
{
	while (! peekistok(l, CACT_TOKEN_END)) {
		printlex(peeklex(l));
		nextlex(l);
	}
	printlex(peeklex(l));
}

/* Convert a lexeme to a symbol. */
cact_val*
lextosym(struct cactus *cact, struct cact_lexeme lx)
{
	char* sym = strslice(lx.st, &lx.st[lx.sz]);
	return cact_get_symbol(cact, sym);
}

/* Convert a lexeme to an integer. */
cact_val*
lextoint(struct cact_lexeme lx)
{
	char *end = lx.st;
	long i = strtol(lx.st, &end, 10);
	return cact_make_integer(i);
}

/* Convert a lexeme to a string. */
cact_val*
lextostr(struct cact_lexeme lx)
{
	return cact_make_string(strslice(lx.st + 1, &lx.st[lx.sz - 1]));
}

/* Convert a lexeme to a boolean. */
cact_val* 
lextobool(struct cact_lexeme lx) 
{
	if (strncmp(lx.st, "#t", 2) == 0 || strncmp(lx.st, "#true", 5) == 0) {
		return cact_make_boolean(true);
	} else if (strncmp(lx.st, "#f", 2) == 0 || strncmp(lx.st, "#false", 6) == 0) {
		return cact_make_boolean(false);
	}
	return cact_make_error("Improperly formatted boolean.", NULL);
}

/* Read a list from the given lexer and fill the cact_val with it. */
int 
readlist(struct cactus *cact, cact_val **r) 
{
    	struct cact_lexer *l = &cact->lexer;
	int status;
	*r = NULL;

	struct cact_lexeme open_paren = peeklex(l);

	if (open_paren.t != CACT_TOKEN_OPEN_PAREN) {
		*r = cact_make_error("readlist: somehow didn't get open paren", NULL);
		return CACT_READ_OTHER_ERROR;
	}

	nextlex(l);

	while (! peekistok(l, CACT_TOKEN_CLOSE_PAREN) && peeklex(l).t > 0) {
		cact_val *e = NULL;
		status = cact_read(cact, &e);
		if (status != CACT_READ_OK) {
			return status;
		}
		if (e == &undefined) {
			*r = cact_make_error("Unexpected ending", NULL);
			return CACT_READ_OTHER_ERROR;
		}
		*r = append(*r, e);
		expecttok(l, CACT_TOKEN_WHITESPACE);
	}

	if (! expecttok(l, CACT_TOKEN_CLOSE_PAREN)) {
		*r = cact_make_error(
			"readlist: somehow didn't get close paren", 
			cact_make_pair(
				cact_make_integer(open_paren.coords.line),
				cact_make_integer(open_paren.coords.col)
			)
		);
		return CACT_READ_UNMATCHED_CHAR;
	}

	return CACT_READ_OK;
}

/* Read the next valid s-expression from the lexer. */
enum cact_read_status cact_read(struct cactus* cact, struct cact_val** ret) 
{
	int status = CACT_READ_IN_PROGRESS;
	*ret = (cact_val*) NULL;
	struct cact_lexer *l = &cact->lexer;

	struct cact_lexeme lx;

	do {
		lx = peeklex(l);
		switch (lx.t) {

		/* Ignored tokens. */

		case CACT_TOKEN_WHITESPACE:
		case CACT_TOKEN_COMMENT:
			nextlex(l);
			break;

		/* Convert the lexeme directly into a value. */

		case CACT_TOKEN_IDENTIFIER:
			*ret = lextosym(cact, lx);
			status = CACT_READ_OK;
			nextlex(l);
			break;
		case CACT_TOKEN_BOOLEAN:
			*ret = lextobool(lx);
			status = CACT_READ_OK;
			nextlex(l);
			break;
		case CACT_TOKEN_INTEGER:
			*ret = lextoint(lx);
			status = CACT_READ_OK;
			nextlex(l);
			break;
		case CACT_TOKEN_STRING:
			*ret = lextostr(lx);
			status = CACT_READ_OK;
			nextlex(l);
			break;

		/* Data structures. */

		case CACT_TOKEN_OPEN_PAREN:
			status = readlist(cact, ret);
			break;

		/* Extra syntax. */

		case CACT_TOKEN_SINGLE_QUOTE:
			nextlex(l);
			cact_val* quoted;
			status = cact_read(cact, &quoted);
			cact_val* q = cact_get_symbol(cact, "quote");
			*ret = cact_make_pair(q, quoted);
			status = CACT_READ_OK;
			break;

		case CACT_TOKEN_END:
			status = CACT_READ_END_OF_FILE;
			break;

		case CACT_TOKEN_ERROR:
		default:
			status = CACT_READ_OTHER_ERROR;
			break;
		}

	} while (status == CACT_READ_IN_PROGRESS);

	return status;
}

