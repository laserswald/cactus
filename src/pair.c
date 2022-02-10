
#include <assert.h>

#include "builtin.h"
#include "core.h"
#include "pair.h"
#include "err.h"

#include "storage/store.h"

/* Create a pair. */
struct cact_val
cact_cons(struct cactus *cact, struct cact_val a, struct cact_val d)
{
    if (cact_is_obj(a)) {
        cact_preserve(cact, a);
    }

    if (cact_is_obj(d)) {
        cact_preserve(cact, d);
    }

    struct cact_pair *p = (struct cact_pair *)cact_alloc(cact, CACT_OBJ_PAIR);
    p->car = a;
    p->cdr = d;

    if (cact_is_obj(a)) {
        cact_unpreserve(cact, a);
    }

    if (cact_is_obj(d)) {
        cact_unpreserve(cact, d);
    }

    return CACT_OBJ_VAL((struct cact_obj *)p);
}

struct cact_val
cact_builtin_cons(struct cactus *cact, struct cact_val args)
{
    struct cact_val a, d;

    if (2 != cact_unpack_args(cact, args, "..", &a, &d)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(a);
    PROPAGATE_ERROR(d);

    return cact_cons(cact, a, d);
}


/* Get the car of a pair. */
struct cact_val
cact_car(struct cactus *cact, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair *p = cact_to_pair(x, "car");
    return p->car;
}

/* Unpack arguments, and take the car of the list at the first argument */
struct cact_val
cact_builtin_car(struct cactus *cact, struct cact_val args)
{
    struct cact_val l;

    if (1 != cact_unpack_args(cact, args, "p", &l)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(l);

    return cact_car(cact, l);
}

/* Get the cdr of a pair. */
struct cact_val
cact_cdr(struct cactus *cact, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair *p = cact_to_pair(x, "cdr");
    return p->cdr;
}

struct cact_val
cact_builtin_cdr(struct cactus *cact, struct cact_val args)
{
    struct cact_val l;

    if (1 != cact_unpack_args(cact, args, "p", &l)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }
    PROPAGATE_ERROR(l);

    return cact_cdr(cact, l);
}

/* Set the car of a pair. */
struct cact_val
cact_set_car(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    struct cact_pair *pr = cact_to_pair(x, "set-car!");
    pr->car = x;
    return CACT_UNDEF_VAL;
}

/* Set the cdr of a pair. */
struct cact_val
cact_set_cdr(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    struct cact_pair *pr = cact_to_pair(x, "set-cdr!");
    pr->cdr = x;
    return CACT_UNDEF_VAL;
}

void
cact_mark_pair(struct cact_obj *o)
{
    struct cact_pair *p = (struct cact_pair *) o;
    cact_mark_val(p->car);
    cact_mark_val(p->cdr);
}

void
cact_destroy_pair(struct cact_obj *o)
{
    return;
}

/* Add a key and value to an association list. */
struct cact_val
cact_list_acons(struct cactus *cact, struct cact_val key, struct cact_val val, struct cact_val alist)
{
    struct cact_val pair = cact_cons(cact, key, val);
    return cact_cons(cact, pair, alist);
}

/* Lookup the value associated with the key in the alist. */
struct cact_val
cact_assoc(struct cactus *cact, struct cact_val key, struct cact_val alist)
{
    if (cact_is_null(alist)) {
        return CACT_NULL_VAL;
    }

    struct cact_val fst = cact_car(cact, alist);
    if (cact_is_error(fst)) {
        return fst;
    }

    struct cact_pair *kv = cact_to_pair(fst, "assoc");

    if (cact_val_equal(kv->car, key)) {
        return cact_car(cact, alist);
    }

    return cact_assoc(cact, key, cact_cdr(cact, alist));
}

struct cact_val
cact_append(struct cactus *cact, struct cact_val l, struct cact_val x)
{
    assert(cact);

    if (cact_is_null(l)) {
        return cact_cons(cact, x, CACT_NULL_VAL);
    }

    if (! cact_is_pair(l)) {
        return cact_make_error(cact, "Tried to append to non-list", CACT_NULL_VAL);
    }

    struct cact_pair *e = (struct cact_pair*) l.as.object;

    while (cact_is_pair(e->cdr)) {
        e = (struct cact_pair*) e->cdr.as.object;
    }

    assert(cact_is_null(e->cdr));
    e->cdr = cact_cons(cact, x, CACT_NULL_VAL);

    return l;
}

unsigned int
cact_length(struct cactus *cact, struct cact_val l)
{
    unsigned int len = 0;

    assert(cact);

    CACT_LIST_FOR_EACH_PAIR(cact, ignore, l) {
        len++;
    }
    return len;
}

