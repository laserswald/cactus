#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"

#include "read.h"
#include "val.h"
#include "sym.h"
#include "str.h"
#include "pair.h"
#include "err.h"

#include "internal/debug.h"
#include "internal/utils.h"

void
cact_str_coords_init(cact_str_coords_t *cds)
{
    memset(cds, 0, sizeof(cact_str_coords_t));
}

void
cact_str_coords_push(cact_str_coords_t *cds, int c)
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
static int
is_initial_identifier(int i)
{
    return i != 0 && (isalpha(i) || strchr("!$%&*/:<=>?@^_~", i) != NULL);
}

static int
is_subsequent_identifier(int i)
{
    return i != 0 && (isalpha(i) || isdigit(i) || strchr("!$%&*./:<=>?@^_~", i) != NULL);
}

/* Return 0 if the character is double quote, 1 otherwise. */
static int
notdblq(int c)
{
    return c != '"';
}

/* Return 0 if the character is newline, 1 otherwise. */
static int
notnl(int c)
{
    return c != '\n';
}

/* Initialize a lexer with a string to parse. */
void
cact_lexer_init(cact_lexer_t* l, const char* s)
{
    l->st = s;
    l->cur = (char*) s;
    cact_str_coords_init(&l->coords);
    l->buf.t = CACT_TOKEN_NOTHING;
}

static int
cact_lexer_getc(cact_lexer_t* l)
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
cact_lexer_peekc(cact_lexer_t* l)
{
    if (! l->cur) {
        return -1;
    }
    return *l->cur;
}

/* Consume characters from the string until the function returns 0. */
static int
cact_lexer_charspan(cact_lexer_t* l, int (*fn)(int))
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
printlex(cact_lexeme_t lx)
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

void
cact_lexer_decimal(cact_lexer_t *l, cact_lexeme_t *le)
{
	le->t = CACT_TOKEN_FLOAT;
    cact_lexer_getc(l);
    cact_lexer_charspan(l, isdigit);
}

void
cact_lexer_unsigned_number(cact_lexer_t *l, cact_lexeme_t *le)
{
	le->t = CACT_TOKEN_INTEGER;
    cact_lexer_charspan(l, isdigit);
    int c = cact_lexer_peekc(l);
    if (c == '.') {
	    cact_lexer_decimal(l, le);
    }
}

void
cact_lexer_minus_and_plus(cact_lexer_t *l, cact_lexeme_t *le)
{
	cact_lexer_getc(l);
    int c = cact_lexer_peekc(l);
    if (isdigit(c)) {
		cact_lexer_unsigned_number(l, le);
    } else {
        le->t = CACT_TOKEN_IDENTIFIER;
        cact_lexer_charspan(l, is_subsequent_identifier);
    }
}

/* Return the next lexeme available from the lexer. */
cact_lexeme_t
nextlex(cact_lexer_t* l)
{
    cact_lexeme_t le = {
        .st = l->cur,
        .coords = l->coords
    };

    int c = cact_lexer_peekc(l);

    if (! c) {
        le.t = CACT_TOKEN_END;
    } else if (isspace(c)) {
        le.t = CACT_TOKEN_WHITESPACE;
        cact_lexer_charspan(l, isspace);
    } else if (is_initial_identifier(c)) {
        le.t = CACT_TOKEN_IDENTIFIER;
        cact_lexer_charspan(l, is_subsequent_identifier);
    } else if (isdigit(c)) {
	    cact_lexer_unsigned_number(l, &le);
    } else if (c == '-' || c == '+') {
	    cact_lexer_minus_and_plus(l, &le);
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
cact_lexeme_t
peeklex(cact_lexer_t *l)
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
peekistok(cact_lexer_t *l, enum cact_token t)
{
    cact_lexeme_t e = peeklex(l);
    return e.t == t;
}

/* If the current lexeme is of the specified token, consume it. */
int
expecttok(cact_lexer_t *l, enum cact_token t)
{
    int istok = peekistok(l, t);
    if (istok) {
        nextlex(l);
    }
    return istok;
}

/* Print the entire stream of tokens from the lexer. */
void
printtokstream(cact_lexer_t *l)
{
    while (! peekistok(l, CACT_TOKEN_END)) {
        printlex(peeklex(l));
        nextlex(l);
    }
    printlex(peeklex(l));
}

/* Convert a lexeme to a symbol. */
cact_value_t
lextosym(cact_context_t *cact, cact_lexeme_t lx)
{
    char* sym = strslice(lx.st, &lx.st[lx.sz]);
    return cact_make_symbol(cact, sym);
}

/* Convert a lexeme to an integer. */
cact_value_t
lextoint(cact_context_t *cact, cact_lexeme_t lx)
{
    char *end = lx.st;
    long i = strtol(lx.st, &end, 10);
    return CACT_FIX_VAL(i);
}

/* Convert a lexeme to a floating point. */
cact_value_t
lextofloat(cact_context_t *cact, cact_lexeme_t lx)
{
    char *end = lx.st;
    double d = strtod(lx.st, &end);
    return CACT_FLO_VAL(d);
}

/* Convert a lexeme to a string. */
cact_value_t
lextostr(cact_context_t *cact, cact_lexeme_t lx)
{
    return cact_make_string(cact, strslice(lx.st + 1, &lx.st[lx.sz - 1]));
}

/* Convert a lexeme to a boolean. */
cact_value_t
lextobool(cact_context_t *cact, cact_lexeme_t lx)
{
    if (strncmp(lx.st, "#t", 2) == 0 || strncmp(lx.st, "#true", 5) == 0) {
        return CACT_BOOL_VAL(true);
    } else if (strncmp(lx.st, "#f", 2) == 0 || strncmp(lx.st, "#false", 6) == 0) {
        return CACT_BOOL_VAL(false);
    }
    return cact_make_error(cact, "Improperly formatted boolean.", CACT_NULL_VAL);
}

/* Read a list from the given lexer and fill the cact_val with it. */
int
readlist(cact_context_t *cact, cact_value_t *r)
{
    assert(cact);
    assert(r);

    cact_lexer_t *l = &cact->lexer;
    int status;

    *r = CACT_NULL_VAL;

    cact_lexeme_t open_paren = peeklex(l);

    if (open_paren.t != CACT_TOKEN_OPEN_PAREN) {
        *r = cact_make_error(cact, "readlist: somehow didn't get open paren", CACT_NULL_VAL);
        return CACT_READ_OTHER_ERROR;
    }

    nextlex(l);

    while (! peekistok(l, CACT_TOKEN_CLOSE_PAREN) && peeklex(l).t > 0) {
        cact_value_t e;
        status = cact_read(cact, &e);
        cact_preserve(cact, e);
        if (status != CACT_READ_OK) {
            return status;
        }
        if (cact_is_undef(e)) {
            *r = cact_make_error(cact, "Unexpected ending", CACT_NULL_VAL);
            return CACT_READ_OTHER_ERROR;
        }

        *r = cact_append(cact, *r, e);
        cact_preserve(cact, *r);
        cact_unpreserve(cact, e);

        expecttok(l, CACT_TOKEN_WHITESPACE);
    }

    if (! expecttok(l, CACT_TOKEN_CLOSE_PAREN)) {
        *r = cact_make_error(
                 cact,
                 "readlist: somehow didn't get close paren",
                 cact_cons(
                     cact,
                     CACT_FIX_VAL(open_paren.coords.line),
                     CACT_FIX_VAL(open_paren.coords.col)
                 )
             );
        return CACT_READ_UNMATCHED_CHAR;
    }

    cact_unpreserve(cact, *r);

    return CACT_READ_OK;
}

/* Read the next valid s-expression from the lexer. */
enum cact_read_status
cact_read(cact_context_t* cact, cact_value_t* ret) {
    int status = CACT_READ_IN_PROGRESS;
    *ret = CACT_NULL_VAL;
    cact_lexer_t *l = &cact->lexer;

    cact_lexeme_t lx;

    do
    {
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
            *ret = lextobool(cact, lx);
            status = CACT_READ_OK;
            nextlex(l);
            break;
        case CACT_TOKEN_INTEGER:
            *ret = lextoint(cact, lx);
            status = CACT_READ_OK;
            nextlex(l);
            break;
        case CACT_TOKEN_FLOAT:
            *ret = lextofloat(cact, lx);
            status = CACT_READ_OK;
            nextlex(l);
            break;
        case CACT_TOKEN_STRING:
            *ret = lextostr(cact, lx);
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
            cact_value_t quoted;
            status = cact_read(cact, &quoted);
            cact_value_t q = cact_make_symbol(cact, "quote");
            *ret = cact_cons(cact, q, quoted);
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

