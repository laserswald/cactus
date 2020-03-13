
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
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    p.as.object->as.pair.car = x;
    return CACT_UNDEF_VAL;
}

/* Set the cdr of a pair. */
struct cact_val
cact_set_cdr(struct cactus *cact, struct cact_val p, struct cact_val x)
{
    if (! cact_is_pair(p)) {
        return cact_make_error(cact, "Not a pair: ", p);
    }

    p.as.object->as.pair.cdr = x;
    return CACT_UNDEF_VAL;
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

    struct cact_pair kv = cact_to_pair(fst, "assoc");

    if (cact_val_equal(kv.car, key)) {
        return cact_car(cact, alist);
    }

    return cact_assoc(cact, key, cact_cdr(cact, alist));
}

struct cact_val
cact_append(struct cactus *cact, struct cact_val l, struct cact_val x)
{
	assert(cact);

    if (cact_is_null(l))
        return cact_cons(cact, x, CACT_NULL_VAL);

    struct cact_val e = l;

    while (! cact_is_null(cact_cdr(cact, e)))
        e = cact_cdr(cact, e);

    cact_set_cdr(cact, e, cact_cons(cact, x, CACT_NULL_VAL));
    return l;
}

unsigned int 
cact_length(struct cactus *cact, struct cact_val l) 
{
    unsigned int len = 0;
    CACT_LIST_FOR_EACH_ITEM(cact, ignore, l) {
        len++;
    }
    return len;
}
