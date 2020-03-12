
#include "cactus/core.h"
#include "cactus/store.h"
#include "cactus/pair.h"

/* Create a pair. */
struct cact_val 
cact_cons(struct cactus *cact, struct cact_val a, struct cact_val d)
{
	struct cact_val v;
	v.type = CACT_TYPE_OBJ;
	struct cact_obj *p = cact_store_allocate(&cact->store);
	p->type = CACT_OBJ_PAIR;
	p->as.pair.car = a;
	p->as.pair.cdr = d;
	v.as.object = p;
    return v;
}

/* Get the car of a pair. */
struct cact_val 
cact_car(struct cactus *cact, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair p = x.as.object->as.pair;
    return p.car;
}

/* Get the cdr of a pair. */
struct cact_val
cact_cdr(struct cactus *cact, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair p = x.as.object->as.pair;
    return p.cdr;
}

/* Set the car of a pair. */
struct cact_val 
cact_set_car(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return cact_make_error(cact, "Not a pair: ", x);
    }

    struct cact_pair p = x.as.object->as.pair;
    p.car = x;
}

/* Set the cdr of a pair. */
struct cact_val
cact_set_cdr(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    struct cact_pair p = x.as.object->as.pair;
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

unsigned int 
cact_length(cact_val *l) 
{
    unsigned int len = 0;
    LIST_FOR_EACH(l, p) {
        len++;
    }
    return len;
}
