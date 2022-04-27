
#include <assert.h>

#include "pair.h"
#include "builtin.h"
#include "core.h"
#include "err.h"

#include "storage/store.h"
#include "storage/obj.h"

/* Create a pair. */
cact_value_t
cact_cons(cact_context_t *cact, cact_value_t a, cact_value_t d)
{
    if (cact_is_obj(a)) {
        cact_preserve(cact, a);
    }

    if (cact_is_obj(d)) {
        cact_preserve(cact, d);
    }

    cact_pair_t *p = (cact_pair_t *)cact_alloc(cact, CACT_OBJ_PAIR);
    p->car = a;
    p->cdr = d;

    if (cact_is_obj(a)) {
        cact_unpreserve(cact, a);
    }

    if (cact_is_obj(d)) {
        cact_unpreserve(cact, d);
    }

    return CACT_OBJ_VAL((cact_object_t *)p);
}

cact_value_t
cact_builtin_cons(cact_context_t *cact, cact_value_t args)
{
    cact_value_t a, d;

    if (2 != cact_unpack_args(cact, args, "..", &a, &d)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(a);
    PROPAGATE_ERROR(d);

    return cact_cons(cact, a, d);
}


/* Get the car of a pair. */
cact_value_t
cact_car(cact_context_t *cact, cact_value_t x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    cact_pair_t *p = cact_to_pair(x, "car");
    return p->car;
}

/* Unpack arguments, and take the car of the list at the first argument */
cact_value_t
cact_builtin_car(cact_context_t *cact, cact_value_t args)
{
    cact_value_t l;

    if (1 != cact_unpack_args(cact, args, "p", &l)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(l);

    return cact_car(cact, l);
}

/* Get the cdr of a pair. */
cact_value_t
cact_cdr(cact_context_t *cact, cact_value_t x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    cact_pair_t *p = cact_to_pair(x, "cdr");
    return p->cdr;
}

cact_value_t
cact_builtin_cdr(cact_context_t *cact, cact_value_t args)
{
    cact_value_t l;

    if (1 != cact_unpack_args(cact, args, "p", &l)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(l);

    return cact_cdr(cact, l);
}

/* Set the car of a pair. */
cact_value_t
cact_set_car(cact_context_t *cact, cact_value_t p, cact_value_t x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    cact_pair_t *pr = cact_to_pair(x, "set-car!");
    pr->car = x;
    return CACT_UNDEF_VAL;
}

/* Set the cdr of a pair. */
cact_value_t
cact_set_cdr(cact_context_t *cact, cact_value_t p, cact_value_t x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    cact_pair_t *pr = cact_to_pair(x, "set-cdr!");
    pr->cdr = x;
    return CACT_UNDEF_VAL;
}

void
cact_mark_pair(cact_object_t *o)
{
    cact_pair_t *p = (cact_pair_t *) o;
    cact_mark_value(p->car);
    cact_mark_value(p->cdr);
}

void
cact_destroy_pair(cact_object_t *o)
{
    return;
}

/* Add a key and value to an association list. */
cact_value_t
cact_list_acons(cact_context_t *cact, cact_value_t key, cact_value_t val, cact_value_t alist)
{
    cact_value_t pair = cact_cons(cact, key, val);
    return cact_cons(cact, pair, alist);
}

/* Lookup the value associated with the key in the alist. */
cact_value_t
cact_assoc(cact_context_t *cact, cact_value_t key, cact_value_t alist)
{
    if (cact_is_null(alist)) {
        return CACT_NULL_VAL;
    }

    cact_value_t fst = cact_car(cact, alist);
    if (cact_is_error(fst)) {
        return fst;
    }

    cact_pair_t *kv = cact_to_pair(fst, "assoc");

    if (cact_value_equal(kv->car, key)) {
        return cact_car(cact, alist);
    }

    return cact_assoc(cact, key, cact_cdr(cact, alist));
}

cact_value_t
cact_append(cact_context_t *cact, cact_value_t l, cact_value_t x)
{
    assert(cact);

    if (cact_is_null(l)) {
        return cact_cons(cact, x, CACT_NULL_VAL);
    }

    if (! cact_is_pair(l)) {
        return cact_make_error(cact, "Tried to append to non-list", CACT_NULL_VAL);
    }

    cact_pair_t *e = (cact_pair_t*) l.as.object;

    while (cact_is_pair(e->cdr)) {
        e = (cact_pair_t*) e->cdr.as.object;
    }

    assert(cact_is_null(e->cdr));
    e->cdr = cact_cons(cact, x, CACT_NULL_VAL);

    return l;
}

unsigned int
cact_length(cact_context_t *cact, cact_value_t l)
{
    unsigned int len = 0;

    assert(cact);

    CACT_LIST_FOR_EACH_PAIR(cact, ignore, l) {
        len++;
    }
    return len;
}

