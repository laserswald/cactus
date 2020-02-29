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

bool is_truthy(struct cact_val *x)
{
    if (is_bool(x) && x->b == false) {
        return false;
    }

    return true;
}


/* Is this sexp nil? */
bool
cact_is_null(cact_val x)
{
    return x.t == CACT_TYPE_NULL;
}

/* 
 * Returns true if the two items 'refer to the same object'.
 * 
 * Semantically, #t, #f, and the empty list are all individual 
 * objects, so comparing booleans and nulls are valid for 'eq?'. 
 * Heap allocated objects will be compared by their pointers.
 */
bool
cact_val_eq(cact_val l, cact_val r)
{
	/* Go ahead and bail if they are different types */
	if (l.t != r.t) {
		return false;
	}

    switch (l.t) {
    case CACT_TYPE_NULL:
		return true;
    case CACT_TYPE_BOOLEAN:
        return l.b == r.b;
    case CACT_TYPE_OBJECT:
        return l.obj == r.obj;
    }

    return false;
}

bool
cact_val_eqv(cact_val l, cact_val r)
{
    if (cact_val_eq(l, r)) {
        return true;
    }

    switch (l.t) {
    case CACT_TYPE_INT:
        return l.i == r.i;
    case CACT_TYPE_DOUBLE:
        return l.f == r.f;
    case CACT_TYPE_CHARACTER:
        return l.c == r.c;
    }

    return false;
}

/* Deeply compare two values. */
bool
cact_val_equal(cact_val l, cact_val r)
{
    if (cact_val_eqv(l, r)) {
        return true;
    }

    if (l.t != CACT_TYPE_OBJECT) {
	    return false;
    }

    cact_obj obj = l.obj;

    switch (obj.) {
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

cact_val *
sexp_not(cact_val *x) 
{
    if (is_bool(x) && x->b == false) {
	    return cact_make_boolean(true);
    }
    return cact_make_boolean(false);
}
