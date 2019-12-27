#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "debug.h"
#include "sexp.h"
#include "read.h"
#include "utils.h"
#include "sym.h"

/* Is this int allowable as an identifier character? */
int
is_ident(int i)
{
    return i != 0 && (isalpha(i) || strchr("!$%&*+-./:<=>?@^_~", i) != NULL);
}

/* Consume characters from the string until the function returns 0. */
int
charspan(const char* s, int (*fn)(int))
{
    int l = 0;
    while (*s && fn(*s)) {
        s++;
        l++;
    }
    return l;
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
lexer_init(struct lexer* l, const char* s)
{
    l->st = s;
    l->cur = (char*) s;
    l->lno = 0;
    l->buf.t = TOKEN_NOTHING;
}

/* Print a lexeme to stdout. */
void
printlex(struct lexeme lx)
{
    switch (lx.t) {
    case TOKEN_ERROR:
        printf("error");
        break;
    case TOKEN_END:
        printf("end");
        break;
    case TOKEN_NOTHING:
        printf("nothing");
        break;
    case TOKEN_WHITESPACE:
        printf("whitespace");
        break;
    case TOKEN_IDENTIFIER:
        printf("identifier");
        break;
    case TOKEN_BOOLEAN:
        printf("boolean");
        break;
    case TOKEN_CHARACTER:
        printf("character");
        break;
    case TOKEN_INTEGER:
        printf("integer");
        break;
    case TOKEN_FLOAT:
        printf("float");
        break;
    case TOKEN_STRING:
        printf("string");
        break;
    case TOKEN_OPEN_PAREN:
        printf("open paren");
        break;
    case TOKEN_CLOSE_PAREN:
        printf("close paren");
        break;
    case TOKEN_SINGLE_QUOTE:
        printf("single quote");
        break;
    case TOKEN_COMMENT:
        printf("comment");
        break;
    default:
        break;
    }

    char* sym = strslice(lx.st, &lx.st[lx.sz]);

    printf(":\t'%s'\n", sym);
}

/* Return the next lexeme available from the lexer. */
struct lexeme
nextlex(struct lexer* l)
{
    struct lexeme le = {
        .st = l->cur,
        .lno = l->lno,
    };

	char *p = l->cur;

	if (! p || ! l->st) {
		le.t = TOKEN_ERROR;
	} else if (isspace(*p)) {
		le.t = TOKEN_WHITESPACE;
		while (*p && isspace(*p)){
			if (*p == '\n') l->lno++;
			p++;
		}
	} else if (is_ident(*p)) {
		le.t = TOKEN_IDENTIFIER;
		p += charspan(p, is_ident);
	} else if (isdigit(*p)) {
		le.t = TOKEN_INTEGER;
		p += charspan(p, isdigit);
	} else if (*p == '(') {
		le.t = TOKEN_OPEN_PAREN;
		p++;
	} else if (*p == ')') {
		le.t = TOKEN_CLOSE_PAREN;
		p++;
	} else if (*p == '\'') {
		le.t = TOKEN_SINGLE_QUOTE;
		p++;
	} else if (*p == '"') {
		le.t = TOKEN_STRING;
		p++;
		p += charspan(p, notdblq);
		p++;
	} else if (*p == '#') { 
		/* boolean and character */
		p++;
		if (*p == 't' || *p == 'f') {
			p++;
			le.t = TOKEN_BOOLEAN;
		} else if (*p == '\\') {
			p++;
			le.t = TOKEN_CHARACTER;
			p += charspan(p, isalpha);
		} else {
			le.t = TOKEN_ERROR;
		}
	} else if (*p == ';') { 
        le.t = TOKEN_COMMENT;
		p += charspan(p, notnl);
		l->lno++;
        p++;
	} else if (! *p) {
		le.t = TOKEN_END;
	}

	le.sz = p - l->cur;
	l->cur = p;
	l->buf = le;

	return le;
}

/* Peek at the current lexeme. */
struct lexeme
peeklex(struct lexer *l)
{
    if (l->buf.t == TOKEN_NOTHING) {
        nextlex(l);
    }
    return l->buf;
}

/*
 * Peek at the current lexeme and return 1 if it is the specified
 * token, or 0 otherwise.
 */
int
peekistok(struct lexer *l, enum token t)
{
    struct lexeme e = peeklex(l);
    return e.t == t;
}

/* If the current lexeme is of the specified token, consume it. */
int
expecttok(struct lexer *l, enum token t)
{
    int istok = peekistok(l, t);
    if (istok) {
        nextlex(l);
    }
    return istok;
}

/* Print the entire stream of tokens from the lexer. */
void
printtokstream(struct lexer *l)
{
    while (! peekistok(l, TOKEN_END)) {
        printlex(peeklex(l));
        nextlex(l);
    }
    printlex(peeklex(l));
}

/* Convert a lexeme to a symbol. */
cact_val*
lextosym(struct lexeme lx)
{
    char* sym = strslice(lx.st, &lx.st[lx.sz]);
    return cact_make_symbol(sym);
}

/* Convert a lexeme to an integer. */
cact_val*
lextoint(struct lexeme lx)
{
    char *end = lx.st;
    long i = strtol(lx.st, &end, 10);
    return cact_make_integer(i);
}

/* Convert a lexeme to a string. */
cact_val*
lextostr(struct lexeme lx)
{
    return cact_make_string(strslice(lx.st + 1, &lx.st[lx.sz - 1]));
}

/* Convert a lexeme to a boolean. */
cact_val* 
lextobool(struct lexeme lx) 
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
readlist(struct lexer *l, cact_val **r) 
{
	int status;
	*r = NULL;
	if (! expecttok(l, TOKEN_OPEN_PAREN)) {
		*r = cact_make_error("readlist: somehow didn't get open paren", NULL);
		return CACT_READ_OTHER_ERROR;
	}
	while (! peekistok(l, TOKEN_CLOSE_PAREN) && peeklex(l).t > 0) {
		cact_val *e = NULL;
		status = cact_read(l, &e);
		if (status != CACT_READ_OK) {
			return status;
		}
		if (e == &undefined) {
			*r = cact_make_error("Unexpected ending", NULL);
			return CACT_READ_OTHER_ERROR;
		}
		*r = append(*r, e);
		expecttok(l, TOKEN_WHITESPACE);
	}
	if (! expecttok(l, TOKEN_CLOSE_PAREN)) {
		*r = cact_make_error("readlist: somehow didn't get close paren", NULL);
		return CACT_READ_OTHER_ERROR;
	}
	return CACT_READ_OK;
}

/* Read the next valid s-expression from the lexer. */
int 
cact_read(struct lexer* l, cact_val** ret) 
{
	int status = CACT_READ_OK;
	*ret = (cact_val*) NULL;

	struct lexeme lx = peeklex(l);

	switch (lx.t) {
	case TOKEN_OPEN_PAREN:
		status = readlist(l, ret);
		break;
	case TOKEN_IDENTIFIER:
		*ret = lextosym(lx);
		nextlex(l);
		break;
	case TOKEN_BOOLEAN:
		*ret = lextobool(lx);
		nextlex(l);
		break;
	case TOKEN_INTEGER:
		*ret = lextoint(lx);
		nextlex(l);
		break;
	case TOKEN_SINGLE_QUOTE:
		nextlex(l);
		cact_val* quoted;
		status = cact_read(l, &quoted);
		cact_val* q = cact_make_symbol("quote");
		*ret = cons(q, quoted);
		nextlex(l);
		break;
	case TOKEN_STRING:
		*ret = lextostr(lx);
		nextlex(l);
		break;
	case TOKEN_END:
		status = CACT_READ_END_OF_FILE;
		break;
        case TOKEN_COMMENT:
	        nextlex(l);
	        status = cact_read(l, ret);
	        break;
	case TOKEN_ERROR:
	default:
		status = CACT_READ_OTHER_ERROR;
		break;
	}

	return status;
}

