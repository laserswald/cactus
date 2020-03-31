#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cactus/val.h"
#include "cactus/env.h"
#include "cactus/sym.h"
#include "cactus/pair.h"
#include "cactus/str.h"
#include "cactus/obj.h"

const char *
cact_val_show_type(enum cact_type t)
{

    switch (t)
    {
    case CACT_TYPE_UNDEF:
        return "undefined";
    case CACT_TYPE_NULL:
        return "null";
    case CACT_TYPE_FIXNUM:
        return "fixnum";
    case CACT_TYPE_FLONUM:
        return "flonum";
    case CACT_TYPE_BOOL:
        return "boolean";
    case CACT_TYPE_CHAR:
        return "character";
    case CACT_TYPE_SYM:
        return "symbol";
    case CACT_TYPE_OBJ:
        return "object";
    }
    return NULL;
}

const char*
cact_type_str(struct cact_val x)
{
    if (cact_is_obj(x))
    {
        return cact_obj_show_type(x.as.object->type);
    }
    return cact_val_show_type(x.type);
}


/* Is this sexp nil? */
bool
cact_is_null(struct cact_val x)
{
    return x.type == CACT_TYPE_NULL;
}

/*
 * Returns true if the two items 'refer to the same object'.
 *
 * Semantically, #t, #f, and the empty list are all individual
 * objects, so comparing booleans and nulls are valid for 'eq?'.
 * Heap allocated objects will be compared by their pointers.
 */
bool
cact_val_eq(struct cact_val l, struct cact_val r)
{
    /* Go ahead and bail if they are different types */
    if (l.type != r.type)
    {
        return false;
    }

    switch (l.type)
    {
    case CACT_TYPE_UNDEF:
        return false;
    case CACT_TYPE_NULL:
        return true;
    case CACT_TYPE_BOOL:
        return l.as.boolean == r.as.boolean;
    case CACT_TYPE_SYM:
        return l.as.symbol == r.as.symbol;
    case CACT_TYPE_OBJ:
        return l.as.object == r.as.object;
    default:
        return false;
    }
}

bool
cact_val_eqv(struct cact_val l, struct cact_val r)
{
    if (cact_val_eq(l, r))
    {
        return true;
    }

    switch (l.type)
    {
    case CACT_TYPE_FIXNUM:
        return l.as.fixnum == r.as.fixnum;
    case CACT_TYPE_FLONUM:
        return l.as.flonum == r.as.flonum;
    case CACT_TYPE_CHAR:
        return l.as.character == r.as.character;
    default:
        return false;
    }
}

/* Deeply compare two values. */
bool
cact_val_equal(struct cact_val l, struct cact_val r)
{
    if (cact_val_eqv(l, r))
    {
        return true;
    }

    if (l.type != CACT_TYPE_OBJ)
    {
        return false;
    }

    struct cact_obj *lo = l.as.object;
    struct cact_obj *ro = r.as.object;

    switch (lo->type)
    {
    case CACT_OBJ_STRING:
        return strcmp(((struct cact_string *)lo)->str, ((struct cact_string *)ro)->str) == 0;
    case CACT_OBJ_PAIR:
    {
        struct cact_pair *lp = (struct cact_pair *) lo;
        struct cact_pair *rp = (struct cact_pair *) ro;
        return (cact_val_equal(lp->car, rp->car))
               && (cact_val_equal(lp->cdr, rp->cdr));
    }
    default:
        break;
    }

    return false;
}

