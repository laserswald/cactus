#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sexp.h"
#include "env.h"
#include "sym.h"

const char *
show_type(cact_type t) {
    switch (t) {
    case TYPE_INT:
        return "int";
    case TYPE_DOUBLE:
        return "float";
    case TYPE_BOOLEAN:
        return "boolean";
    case TYPE_CLOSURE:
        return "closure";
    case TYPE_ENVIRONMENT:
        return "environment";
    case TYPE_PAIR:
        return "pair";
    case TYPE_STRING:
        return "string";
    case TYPE_SYMBOL:
        return "symbol";
    case TYPE_PORT:
        return "port";
    case TYPE_ERROR:
        return "error";
    case TYPE_UNDEF:
        return "undefined";
    }
    return NULL;
}

/* Defined constants. */
cact_val undefined = {.t = TYPE_UNDEF};

/* Is this sexp nil? */
bool
is_nil(cact_val *x)
{
    return x == NULL;
}

/* Deeply compare two values. */
bool
equals(cact_val *l, cact_val *r)
{
    if (l->t != r->t) {
        return false;
    }

    switch (l->t) {
    case TYPE_INT:
        return l->i == r->i;
        break;
    case TYPE_DOUBLE:
        return l->f == r->f;
        break;
    case TYPE_BOOLEAN:
        return l->b == r->b;
        break;
    case TYPE_STRING:
        return strcmp(l->s.str, r->s.str) == 0;
        break;
    case TYPE_SYMBOL: {
        cact_symbol lsym = to_sym(l, "equals");
        cact_symbol rsym = to_sym(r, "equals");
        return symcmp(&lsym, &rsym) == 0;
        break;
    }
    case TYPE_PAIR:
        return (equals(car(l), car(r))) && (equals(cdr(l), cdr(r)));
        break;
    case TYPE_CLOSURE:
        break;
    case TYPE_ENVIRONMENT:
        break;
    case TYPE_PORT:
        break;
    case TYPE_ERROR:
        break;
    case TYPE_UNDEF:
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
make_integer(int i)
{
    cact_val *x = malloc(sizeof(cact_val));
    x->t = TYPE_INT;
    x->i = i;
    return x;
}

/* Create a new symbol. */
cact_val *
make_symbol(char *str)
{
    cact_val *sym = malloc(sizeof(cact_val));
    sym->t = TYPE_SYMBOL;
    sym->a.sym = str;
    return sym;
}

/* Create a string. */
cact_val *
make_string(char *str)
{
    cact_val *x = malloc(sizeof(cact_val));
    x->t = TYPE_STRING;
    x->s.str = str;
    return x;
}

/* Create a closure. */
cact_val *
make_closure(cact_env *e, cact_val *argl, cact_val *body)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = TYPE_CLOSURE;
    x->c = calloc(1, sizeof(cact_proc));
    x->c->env = e;
    x->c->body = body;
    x->c->argl = argl;
    return x;
}

/* Create an environment. */
cact_val *
make_env(cact_env *parent)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = TYPE_ENVIRONMENT;
    x->e = calloc(1, sizeof(cact_env));
    x->e->parent = parent;
    return x;
}

/* Create an error value. */
cact_val *
make_error(char *msg, cact_val *irr)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = TYPE_ERROR;
    x->x.msg = strdup(msg);
    x->x.ctx = irr;
    return x;
}

/* Create an error value. */
cact_val *
make_boolean(bool b)
{
    cact_val *x = calloc(1, sizeof(cact_val));
    x->t = TYPE_BOOLEAN;
    x->b = b;
    return x;
}

/* Create a pair. */
cact_val *
cons(cact_val *a, cact_val *d)
{
    cact_val *pair = malloc(sizeof(cact_val));
    pair->t = TYPE_PAIR;
    pair->p.car = a;
    pair->p.cdr = d;
    return pair;
}

/* Get the car of a pair. */
cact_val *
car(cact_val *x)
{
    if (!x && !is_pair(x)) {
        return make_error("Not a pair: ", x);
    }
    cact_pair p = x->p;
    return p.car;
}

/* Get the cdr of a pair. */
cact_val *
cdr(cact_val *x)
{
    if (!x || !is_pair(x)) {
        return make_error("Not a pair: ", x);
    }
    cact_pair p = x->p;
    return p.cdr;
}

/* Add a key and value to an association list. */
cact_val *
acons(cact_val *key, cact_val *val, cact_val *alist)
{
    cact_val *pair = cons(key, val);
    return cons(pair, alist);
}

/* Lookup the value associated with the key in the alist. */
cact_val *
assoc(cact_val *key, cact_val *alist)
{
    if (! alist) {
        return NULL;
    }

    cact_val* fst = car(alist);
    if (is_error(fst)) {
        return fst;
    }

    cact_pair kv = to_pair(fst, "assoc");

    if (equals(kv.car, key)) {
        return car(alist);
    }

    return assoc(key, cdr(alist));
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
	    return make_boolean(true);
    }
    return make_boolean(false);
}
