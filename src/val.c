#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "val.h"
#include "env.h"
#include "sym.h"
#include "pair.h"
#include "str.h"
#include "err.h"

#include "storage/obj.h"
#include "builtin.h"

const char *
cact_value_show_type(enum cact_type t)
{

    switch (t) {
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
cact_type_str(cact_value_t x)
{
    if (cact_is_obj(x)) {
        return cact_obj_show_type(x.as.object->type);
    }
    return cact_value_show_type(x.type);
}


/* Is this sexp nil? */
bool
cact_is_null(cact_value_t x)
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
cact_value_eq(cact_value_t l, cact_value_t r)
{
    /* Go ahead and bail if they are different types */
    if (l.type != r.type) {
        return false;
    }

    switch (l.type) {
    case CACT_TYPE_UNDEF:
        return false;
    case CACT_TYPE_NULL:
        return true; // because we already checked above for the same type
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
DEFINE_COMPARISON_BUILTIN(cact_builtin_eq, cact_value_eq)

bool
cact_value_eqv(cact_value_t l, cact_value_t r)
{
    if (cact_value_eq(l, r)) {
        return true;
    }

    switch (l.type) {
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
DEFINE_COMPARISON_BUILTIN(cact_builtin_eqv, cact_value_eqv)

/* Deeply compare two values. */
bool
cact_value_equal(cact_value_t l, cact_value_t r)
{
    if (cact_value_eqv(l, r)) {
        return true;
    }

    if (l.type != CACT_TYPE_OBJ) {
        return false;
    }

    cact_object_t *lo = l.as.object;
    cact_object_t *ro = r.as.object;

    switch (lo->type) {
    case CACT_OBJ_STRING:
        return strcmp(((cact_string_t *)lo)->str, ((cact_string_t *)ro)->str) == 0;
    case CACT_OBJ_PAIR: {
        cact_pair_t *lp = (cact_pair_t *) lo;
        cact_pair_t *rp = (cact_pair_t *) ro;
        return (cact_value_equal(lp->car, rp->car))
               && (cact_value_equal(lp->cdr, rp->cdr));
    }
    default:
        break;
    }

    return false;
}

DEFINE_COMPARISON_BUILTIN(cact_builtin_equal, cact_value_equal)
void
cact_mark_value(cact_value_t v)
{
    if (! cact_is_obj(v)) {
        return;
    }
    cact_mark_object(v.as.object);
}

void
cact_destroy_value(cact_value_t v)
{
    if (cact_is_obj(v)) {
        cact_destroy_object(cact_to_obj(v, "cact-destroy-val"));
    }
}

