#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#include "sexp.h"
#include "read.h"
#include "utils.h"
#include "sym.h"

enum token {
	TOKEN_ERROR = -2,
	TOKEN_END = -1,
	TOKEN_NOTHING = 0,
	TOKEN_WHITESPACE,
	TOKEN_IDENTIFIER,
	TOKEN_BOOLEAN,
	TOKEN_CHARACTER,
	TOKEN_INTEGER,
	TOKEN_FLOAT,
	TOKEN_STRING,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_SINGLE_QUOTE,
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
	    .st = (char*)l->st,
	    .lno = l->lno,
	};

	char *p = l->cur;

    if (isspace(*p)) {
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

}

Sexp* read_integer(char **str) {
	char *end = *str;
	long i = strtol(*str, &end, 10);
	*str = end;
	return make_integer(i);
}

int readlist(struct lexer *l, Sexp **r) {
	DBG("Reading list.\n");
	int status;

	*r = NULL;

	if (! expecttok(l, TOKEN_OPEN_PAREN)) {
		*r = make_error("readlist: somehow didn't get open paren", NULL);
		return READSEXP_OTHER_ERROR;
	}

	expecttok(l, TOKEN_WHITESPACE);

	if (peektok(l, TOKEN_CLOSE_PAREN)) {
		return READSEXP_OK;
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

int read_sexp(struct lexer* l, Sexp** ret) 
{
	int status = READSEXP_OK;
	*ret = (Sexp*) NULL;


	if (peektok(l, TOKEN_OPEN_PAREN)) {
		DBG("Dispatching on (\n");
		status = readlist(l, ret);
	} else if (peektok(l, TOKEN_IDENTIFIER)) {
        struct lexeme* lxme = peeklex(l);
		*ret = lexeme_to_symbol(lxme);
	} else if (isdigit(**str)) {
		*ret = read_integer(str);
	} else if (**str == '\'') {
		DBG("Dispatching on quote\n");
		(*str)++;

		Sexp* quoted;
		status = readsexp(str, &quoted);

		Sexp* q = make_symbol("quote");
		*ret = cons(q, quoted);
	} else if (**str == '"') {
		*ret = read_string(str);
	} else if (! **str) {
		DBG("Found end of string\n");
		return READSEXP_END_OF_FILE;
	}

	DBG("Rest of string: %s\n", *str);
	return status;
}

