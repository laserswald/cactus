#include "pair.h"

/* Create a pair. */
struct cact_val cact_cons(struct cactus *cact, struct cact_val a, struct cact_val d)
{
	struct cact_val v;
	v.t = CACT_TYPE_OBJECT;
	v.obj = cact_store_allocate(cact.store, CACT_OBJ_PAIR);
	v.obj->pair.car = a;
	v.obj->pair.cdr = d;
    return v;
}

/* Get the car of a pair. */
struct cact_val car(cact_val x)
{
    if (!x && !is_pair(x)) {
        return cact_make_error("Not a pair: ", x);
    }
    cact_pair p = x->p;
    return p.car;
}

/* Get the cdr of a pair. */
cact_val *
cdr(cact_val *x)
{
    if (!x || !is_pair(x)) {
        return cact_make_error("Not a pair: ", x);
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

    if (cact_val_equal(kv.car, key)) {
        return car(alist);
    }

    return assoc(key, cdr(alist));
}
