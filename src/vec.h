#ifndef CACT_VEC_H
#define CACT_VEC_H

#include "core.h"

#include "storage/obj.h"
#include "val.h"

struct cact_vec {
	struct cact_obj obj;
	unsigned long length;
	struct cact_val *items;
};

struct cact_val cact_make_vec_filled(struct cactus *cact, unsigned long length, struct cact_val fill);
struct cact_val cact_make_vec_empty(struct cactus *, unsigned long);
void cact_mark_vec(struct cact_obj *v);
void cact_destroy_vec(struct cact_obj *v);
struct cact_val cact_vec_ref(struct cactus *cact, struct cact_vec *v, unsigned long index);
void cact_vec_set(struct cactus *cact, struct cact_vec *v, unsigned long index, struct cact_val new);
struct cact_val cact_vec_len(struct cactus *cact, struct cact_vec *v);

DEFINE_OBJECT_CONVERSION(CACT_OBJ_VECTOR, struct cact_vec*, cact_to_vec, vec)
DEFINE_OBJECT_CHECK(cact_is_vector, CACT_OBJ_VECTOR)

#endif
