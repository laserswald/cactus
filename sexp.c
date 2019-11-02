#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sexp.h"
#include "env.h"
#include "sym.h"

const char *
show_type(Type t) {
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
Sexp undefined = {.t = TYPE_UNDEF};

/* Is this sexp nil? */
bool
is_nil(Sexp *x)
{
    return x == NULL;
}

/* Deeply compare two values. */
bool
equals(Sexp *l, Sexp *r)
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
        Symbol lsym = to_sym(l, "equals");
        Symbol rsym = to_sym(r, "equals");
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

unsigned int length(Sexp *l) {
    unsigned int len = 0;
    LIST_FOR_EACH(l, p) {
        len++;
    }
    return len;
}

/* Create a new integer. */
Sexp *
make_integer(int i)
{
    Sexp *x = malloc(sizeof(Sexp));
    x->t = TYPE_INT;
    x->i = i;
    return x;
}

/* Create a new symbol. */
Sexp *
make_symbol(char *str)
{
    Sexp *sym = malloc(sizeof(Sexp));
    sym->t = TYPE_SYMBOL;
    sym->a.sym = str;
    return sym;
}

/* Create a string. */
Sexp *
make_string(char *str)
{
    Sexp *x = malloc(sizeof(Sexp));
    x->t = TYPE_STRING;
    x->s.str = str;
    return x;
}

/* Create a closure. */
Sexp *
make_closure(Env *e, Sexp *argl, Sexp *body)
{
    Sexp *x = calloc(1, sizeof(Sexp));
    x->t = TYPE_CLOSURE;
    x->c = calloc(1, sizeof(Closure));
    x->c->env = e;
    x->c->body = body;
    x->c->argl = argl;
    return x;
}

/* Create an environment. */
Sexp *
make_env(Env *parent)
{
    Sexp *x = calloc(1, sizeof(Sexp));
    x->t = TYPE_ENVIRONMENT;
    x->e = calloc(1, sizeof(Env));
    x->e->parent = parent;
    return x;
}

/* Create an error value. */
Sexp *
make_error(char *msg, Sexp *irr)
{
    Sexp *x = calloc(1, sizeof(Sexp));
    x->t = TYPE_ERROR;
    x->x.msg = strdup(msg);
    x->x.ctx = irr;
    return x;
}

/* Create an error value. */
Sexp *
make_boolean(bool b)
{
    Sexp *x = calloc(1, sizeof(Sexp));
    x->t = TYPE_BOOLEAN;
    x->b = b;
    return x;
}

/* Create a pair. */
Sexp *
cons(Sexp *a, Sexp *d)
{
    Sexp *pair = malloc(sizeof(Sexp));
    pair->t = TYPE_PAIR;
    pair->p.car = a;
    pair->p.cdr = d;
    return pair;
}

/* Get the car of a pair. */
Sexp *
car(Sexp *x)
{
    if (!x && !is_pair(x)) {
        return make_error("Not a pair: ", x);
    }
    Pair p = x->p;
    return p.car;
}

/* Get the cdr of a pair. */
Sexp *
cdr(Sexp *x)
{
    if (!x || !is_pair(x)) {
        return make_error("Not a pair: ", x);
    }
    Pair p = x->p;
    return p.cdr;
}

/* Add a key and value to an association list. */
Sexp *
acons(Sexp *key, Sexp *val, Sexp *alist)
{
    Sexp *pair = cons(key, val);
    return cons(pair, alist);
}

/* Lookup the value associated with the key in the alist. */
Sexp *
assoc(Sexp *key, Sexp *alist)
{
    if (! alist) {
        return NULL;
    }

    Sexp* fst = car(alist);
    if (is_error(fst)) {
        return fst;
    }

    Pair kv = to_pair(fst, "assoc");

    if (equals(kv.car, key)) {
        return car(alist);
    }

    return assoc(key, cdr(alist));
}

/* Compare two symbols for equality/ordering. */
int
symcmp(Symbol *a, Symbol *b)
{
    return strcmp(a->sym, b->sym);
}

Sexp *
append(Sexp *l, Sexp *x)
{
    if (!l)
        return cons(x, NULL);

    Sexp *e = l;

    while (cdr(e))
        e = cdr(e);

    e->p.cdr = cons(x, NULL);
    return l;
}


Sexp *
lookup(Env *e, Sexp *key)
{
    if (!e) return NULL;

    Sexp *found_kv = assoc(key, e->list);

    if (!found_kv && e->parent) {
        found_kv = lookup(e->parent, key);
    }

    if (!found_kv) {
        return NULL;
    }

    return found_kv;
}

void
define(Env *e, Sexp *key, Sexp *val)
{
    if (!e)
        return;

    Sexp *found_kv = lookup(e, key);

    if (found_kv) {
        found_kv->p.cdr = val;
    } else {
        e->list = acons(key, val, e->list);
    }
}

Sexp *
sexp_not(Sexp *x) {
    
}
