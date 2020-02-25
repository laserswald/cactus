#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sexp.h"
#include "env.h"
#include "sym.h"

const char *
show_type(cact_type t) {
    switch (t) {
    case CACT_TYPE_INT:
        return "int";
    case CACT_TYPE_DOUBLE:
        return "float";
    case CACT_TYPE_BOOLEAN:
        return "boolean";
    case CACT_TYPE_PROCEDURE:
        return "procedure";
    case CACT_TYPE_ENVIRONMENT:
        return "environment";
    case CACT_TYPE_PAIR:
        return "pair";
    case CACT_TYPE_STRING:
        return "string";
    case CACT_TYPE_SYMBOL:
        return "symbol";
    case CACT_TYPE_PORT:
        return "port";
    case CACT_TYPE_ERROR:
        return "error";
    case CACT_TYPE_UNDEF:
        return "undefined";
    }
    return NULL;
}

/* Defined constants. */
cact_val undefined = {.t = CACT_TYPE_NULL};

/* Is this sexp nil? */
bool is_nil(cact_val x)
{
    return x.t == CACT_TYPE_NULL;
}

bool cact_val_eq(cact_val l, cact_val r)
{
    return l == r;
}

bool cact_val_eqv(cact_val l, cact_val r)
{
    if (cact_val_eq(l, r)) {
        return true;
    }

    if (l.t != r.t) {
        return false;
    }

    switch (l.t) {
    case CACT_TYPE_BOOLEAN:
        return l.b == r.b;
        break;
    case CACT_TYPE_SYMBOL: {
        cact_symbol *lsym = to_sym(l, "equals");
        cact_symbol *rsym = to_sym(r, "equals");
        return symcmp(lsym, rsym) == 0;
        break;
    }
    case CACT_TYPE_INT:
        return l->i == r->i;
        break;
    case CACT_TYPE_DOUBLE:
        return l->f == r->f;
        break;
    case CACT_TYPE_PROCEDURE:
        return l->c == r->c;
        break;
    default:
        break;
    }

    return false;
}

/* Deeply compare two values. */
bool
cact_val_equal(cact_val *l, cact_val *r)
{
    if (cact_val_eqv(l, r)) {
        return true;
    }

    switch (l->t) {
    case CACT_TYPE_STRING:
        return strcmp(l->s.str, r->s.str) == 0;
        break;
    case CACT_TYPE_PAIR:
        return (cact_val_equal(car(l), car(r))) && (cact_val_equal(cdr(l), cdr(r)));
        break;
    case CACT_TYPE_ENVIRONMENT:
        break;
    case CACT_TYPE_PORT:
        break;
    case CACT_TYPE_ERROR:
        break;
    case CACT_TYPE_UNDEF:
        break;
    default:
        break;
    }
    return false;
}

unsigned int length(cact_val *l) {
    unsigned int len = 0;
    LIST_FOR_EACH(l, p) {
        len++;
    }
    return len;
}

/* Create a new integer. */
cact_val *
cact_make_integer(int i)
{
    cact_val *x = malloc(sizeof(cact_val));
    x->t = CACT_TYPE_INT;
    x->i = i;
    return x;
}

/* Create a string. */
cact_val *
cact_make_string(char *str)
{
    cact_val *x = malloc(sizeof(cact_val));
    x->t = CACT_TYPE_STRING;
    x->s.str = str;
    return x;
}

/* Create a procedure. */
cact_val *
cact_make_procedure(cact_env *e, cact_val *argl, cact_val *body)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = CACT_TYPE_PROCEDURE;
    x->c = calloc(1, sizeof(cact_proc));
    x->c->env = e;
    x->c->body = body;
    x->c->argl = argl;
    return x;
}

/* Create an environment. */
cact_val *
cact_make_env(cact_env *parent)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = CACT_TYPE_ENVIRONMENT;
    x->e = calloc(1, sizeof(cact_env));
    x->e->parent = parent;
    return x;
}

/* Create an error value. */
cact_val *
cact_make_error(char *msg, cact_val *irr)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = CACT_TYPE_ERROR;
    x->x.msg = strdup(msg);
    x->x.ctx = irr;
    return x;
}

/* Create an error value. */
cact_val *
cact_make_boolean(bool b)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = CACT_TYPE_BOOLEAN;
    x->b = b;
    return x;
}


/* Compare two symbols for equality/ordering. */
int
symcmp(cact_symbol *a, cact_symbol *b)
{
    return strcmp(a->sym, b->sym);
}

cact_val *
append(cact_val *l, cact_val *x)
{
    if (!l)
        return cons(x, NULL);

    cact_val *e = l;

    while (cdr(e))
        e = cdr(e);

    e->p.cdr = cons(x, NULL);
    return l;
}


cact_val *
lookup(cact_env *e, cact_val *key)
{
    if (!e) return NULL;

    cact_val *found_kv = assoc(key, e->list);

    if (!found_kv && e->parent) {
        found_kv = lookup(e->parent, key);
    }

    if (!found_kv) {
        return NULL;
    }

    return found_kv;
}

void
define(cact_env *e, cact_val *key, cact_val *val)
{
    if (!e)
        return;

    cact_val *found_kv = lookup(e, key);

    if (found_kv) {
        found_kv->p.cdr = val;
    } else {
        e->list = acons(key, val, e->list);
    }
}

cact_val *
sexp_not(cact_val *x) 
{
    if (is_bool(x) && x->b == false) {
	    return cact_make_boolean(true);
    }
    return cact_make_boolean(false);
}
