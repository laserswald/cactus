#ifndef CACT_OBJ_H
#define CACT_OBJ_H

#include "store.h"

/* Types of heap-allocated objects. */
enum cact_obj_type {
	CACT_OBJ_STRING,
	CACT_OBJ_PAIR,
	CACT_OBJ_PROCEDURE,
	CACT_OBJ_ENVIRONMENT,
	/* CACT_OBJ_RECORD_TYPE, */ 
	/* CACT_OBJ_RECORD_INST, */
};

typedef void (*cact_obj_mark_fn)(struct cact_obj *);
typedef void (*cact_obj_destroy_fn)(struct cact_obj *);

/* Common things to do to heap-allocated objects. */
/*
struct cact_obj_ops {
	cact_obj_mark_fn mark;
	cact_obj_destroy_fn destroy;
} cact_obj_operations[] = {
    [CACT_OBJ_STRING] = {cact_string_mark, cact_string_destroy},
    [CACT_OBJ_PAIR] = {cact_pair_mark, cact_pair_destroy},
    [CACT_OBJ_PROCEDURE] = {cact_proc_mark, cact_proc_destroy},
};
*/
void cact_obj_mark(struct cact_obj *);

void cact_obj_destroy(struct cact_obj *);

/* Fundamental data structure for heap-allocated objects. */
struct cact_obj {
    struct cact_store_data store_data;
    enum cact_obj_type type;
};

#define DEFINE_OBJECT_CONVERSION(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(cact_val *x, char *extras) \
{ \
 	if (!x) { \
		fprintf(stderr, "%s: Expected %s, but got nil.\n", extras, show_type(typemarker)); \
		abort(); \
 	} \
	if (x->t != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, show_type(typemarker), show_type(x->t)); \
		abort(); \
	} \
	return x->membername; \
}

#define DEFINE_OBJECT_CHECK(funcname, typemarker) \
static inline bool \
funcname(cact_val *x) { \
	return x && x->t == CACT_TYPE_OBJECT && x->obj.type == typemarker; \
}

#endif // CACT_OBJ_H

