#include "cactus/vec.h"

/*
 * Base vector constructor. 
 * 
 * Build a vector of the specified length and filled with the specified item.
 * This is analogous to `make-vector len fill`. 
 */
struct cact_val 
cact_make_vec_filled(struct cactus *cact, unsigned long length, struct cact_val fill)
{
    if (cact_is_obj(fill)) {
        cact_preserve(cact, fill);
    }

    struct cact_vec *v = (struct cact_vec *) cact_alloc(cact, CACT_OBJ_VECTOR);

    v->items = xcalloc(length, sizeof (struct cact_val));
    v->length = length;

	for (int i = 0; i < v->length; i++) {
		v->items[i] = fill;
	}

    if (cact_is_obj(fill)) {
        cact_unpreserve(cact, fill);
    }

    return CACT_OBJ_VAL(v);
}

/*
 * Simplified vector constructor.
 * 
 * This does exactly the same thing as `cact_make_vec_filled`, but fills the
 * vector with the undefined value.
 */
struct cact_val 
cact_make_vec_empty(struct cactus *cact, unsigned long length)
{
    return cact_make_vec_filled(cact, length, CACT_UNDEF_VAL);
}

/*
 * Vector garbage collection marking function.
 * 
 * If a vector is accessible, all items it refers to are also accessible, so
 * all of the elements within a vector are marked.
 */
void 
cact_mark_vec(struct cact_obj *obj)
{
	struct cact_vec *v = (struct cact_vec *) obj;

	for (int i = 0; i < v->length; i++) {
		cact_mark_val(v->items[i]);
	}
}

/*
 * Vector garbage collection destruction function.
 * 
 * When a vector is inaccessible, it's elements are NOT necessarily inaccessible,
 * since an accessible item can be referred to by other parts of the system. 
 * 
 * Because of this, instead of destroying all the elements of a vector, we simply 
 * free the backing array; values contained within are already either base values
 * (aka integers, bools, null, etc.) or object references.
 */
void 
cact_destroy_vec(struct cact_obj *obj)
{
	struct cact_vec *v = (struct cact_vec *) obj;

	v->length = 0;
	xfree(v->items);
}

/*
 * Refer to a vector element.
 * 
 * This is literally array access. Nothing special to see here. 
 */
struct cact_val 
cact_vec_ref(struct cactus *cact, struct cact_vec *v, unsigned long index)
{
	if (index >= v->length) {
		// TODO: replace this with a raised exception
		fprintf(stderr, "vector-ref: index outside of vector range");
		abort();
	}

	return v->items[index];
}

/*
 * Assign a new value to a vector element.
 * 
 * This is array assignment. Easy cheesy.
 */
void
cact_vec_set(struct cactus *cact, struct cact_vec *v, unsigned long index, struct cact_val new)
{
	if (index >= v->length) {
		// TODO: replace this with a raised exception
		fprintf(stderr, "vector-ref: index outside of vector range");
		abort();
	}

	v->items[index] = new;
}

/*
 * Access the length of this vector.
 */
struct cact_val 
cact_vec_len(struct cactus *cact, struct cact_vec *v)
{
	return CACT_FIX_VAL(v->length);
}

