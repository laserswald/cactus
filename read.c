#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#include "sexp.h"
#include "read.h"
#include "utils.h"
#include "sym.h"

/* Is this int allowable as an identifier character? */
int is_ident(int i) {
	return isalpha(i) || i == '?' || i == '-' || i == '!' || i == '+' || i == '*';
}

int charspan(const char* s, int (*fn)(int)) {
	int l = 0;
	while (*s && fn(*s)){
		s++;
		l++;
	}
	return l;
}

/* not doublequote */
int notdblq(int c) {
	return c != '"';
}

void lexer_init(struct lexer* l, const char* s) {
    l->st = s;
    l->cur = (char*) s;
    l->lno = 0;
    l->buf.t = TOKEN_ERROR;
}

struct lexeme nextlex(struct lexer* l) {
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
    } else if (! *p) {
	    le.t = TOKEN_END;
    }

    le.sz = p - l->cur;
    l->cur = p;
    l->buf = le;

    return le;
}

struct lexeme peeklex(struct lexer *l) {
	return l->buf;
}

int peektok(struct lexer *l, enum token t) {
    struct lexeme e = peeklex(l);
    if (e.t != t) {
        return 0;
    }
    return 1;
}

int expecttok(struct lexer *l, enum token t) {
	int istok = peektok(l, t);
	if (istok) {
        nextlex(l);
	}
	return istok;
}

Sexp* lexeme_to_symbol(struct lexeme lx) {
	char* sym = strslice(lx.st, &lx.st[lx.sz]);
    return make_symbol(sym);
}

Sexp* lexeme_to_int(struct lexeme lx) {
	char *end = lx.st;
	long i = strtol(lx.st, &end, 10);
	return make_integer(i);
}

Sexp* lexeme_to_string(struct lexeme lx) {
	return make_string(strslice(lx.st, &lx.st[lx.sz]));
}

int readlist(struct lexer *l, Sexp **r) {
	DBG("Reading list.\n");
	int status;

	*r = NULL;

	if (! expecttok(l, TOKEN_OPEN_PAREN)) {
		*r = make_error("readlist: somehow didn't get open paren", NULL);
		return READSEXP_OTHER_ERROR;
	}

	while (! peektok(l, TOKEN_CLOSE_PAREN)) {
		Sexp *e = NULL;
		status = readsexp(l, &e);
		if (status != READSEXP_OK) {
			return status;
		}

		if (e == &undefined) {
			*r = make_error("Unexpected ending", NULL);
			return READSEXP_OTHER_ERROR;
		}
		*r = append(*r, e);

		expecttok(l, TOKEN_WHITESPACE);
	}

	if (! expecttok(l, TOKEN_CLOSE_PAREN)) {
		*r = make_error("readlist: somehow didn't get close paren", NULL);
		return READSEXP_OTHER_ERROR;
	}

	DBG("Done reading list.\n");

	return READSEXP_OK;
}

int readsexp(struct lexer* l, Sexp** ret) 
{
	int status = READSEXP_OK;
	*ret = (Sexp*) NULL;

	struct lexeme lx = nextlex(l);

	switch (lx.t) {
	case TOKEN_OPEN_PAREN:
		status = readlist(l, ret);
		break;
	case TOKEN_IDENTIFIER:
		*ret = lexeme_to_symbol(nextlex(l));
		break;
	case TOKEN_INTEGER:
		*ret = lexeme_to_int(nextlex(l));
		break;
	case TOKEN_SINGLE_QUOTE:
		DBG("Dispatching on quote\n");
		nextlex(l);

		Sexp* quoted;
		status = readsexp(l, &quoted);

		Sexp* q = make_symbol("quote");
		*ret = cons(q, quoted);
		break;
	case TOKEN_STRING:
		*ret = lexeme_to_string(nextlex(l));
		break;
	case TOKEN_END:
		status = READSEXP_END_OF_FILE;
		break;
	case TOKEN_ERROR:
	default:
        status = READSEXP_OTHER_ERROR;
		break;
	}

	return status;
}

