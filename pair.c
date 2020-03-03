#include "pair.h"

/* Create a pair. */
struct cact_val 
cact_cons(struct cactus *cact, struct cact_val a, struct cact_val d)
{
	struct cact_val v;
	v.t = CACT_TYPE_OBJECT;
	v.obj = cact_store_allocate(&cact->store);
	v.obj->as.pair.car = a;
	v.obj->as.pair.cdr = d;
    return v;
}

/* Get the car of a pair. */
struct cact_val 
cact_car(struct cactus *cact, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair p = x.obj->as.pair;
    return p.car;
}

/* Get the cdr of a pair. */
struct cact_val
cact_cdr(struct cactus *cact, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair p = x.obj->as.pair;
    return p.cdr;
}

/* Set the car of a pair. */
struct cact_val 
cact_set_car(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair p = x.obj->as.pair;
    p.car = x;
}

/* Set the cdr of a pair. */
struct cact_val
cact_set_cdr(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    struct cact_pair p = x.obj->as.pair;
    p.cdr = x;
}

/* Add a key and value to an association list. */
struct cact_val
cact_list_acons(struct cactus *cact, cact_val key, cact_val val, cact_val alist)
{
    cact_val *pair = cact_cons(key, val);
    return cons(pair, alist);
}

/* Lookup the value associated with the key in the alist. */
struct cact_val
cact_assoc(cact_val *key, cact_val *alist)
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

struct cact_val
cact_append(struct cactus *cact, struct cact_val *l, struct cact_val *x)
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
unsigned int length(cact_val *l) {
    unsigned int len = 0;
    LIST_FOR_EACH(l, p) {
        len++;
    }
    return len;
}
