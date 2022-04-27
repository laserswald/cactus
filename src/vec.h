#ifndef CACT_VEC_H
#define CACT_VEC_H

#include "core.h"

#include "storage/obj.h"
#include "val.h"

typedef struct cact_vec {
	cact_object_t obj;
	unsigned long length;
	cact_value_t *items;
} cact_vec_t;

cact_value_t 
cact_make_vec_filled(cact_context_t *cact, unsigned long length, cact_value_t fill);

cact_value_t 
cact_make_vec_empty(cact_context_t *, unsigned long);

void 
cact_mark_vec(cact_object_t *v);

void 
cact_destroy_vec(cact_object_t *v);

cact_value_t 
cact_vec_ref(cact_context_t *cact, cact_vec_t *v, unsigned long index);

void 
cact_vec_set(cact_context_t *cact, cact_vec_t *v, unsigned long index, cact_value_t new);

cact_value_t 
cact_vec_len(cact_context_t *cact, cact_vec_t *v);

DEFINE_OBJECT_CONVERSION(CACT_OBJ_VECTOR, cact_vec_t *, cact_to_vec, vec)
DEFINE_OBJECT_CHECK(cact_is_vector, CACT_OBJ_VECTOR)

#endif
