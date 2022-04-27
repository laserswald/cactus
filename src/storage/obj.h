#ifndef CACT_OBJ_H
#define CACT_OBJ_H

typedef struct cact_object cact_object_t;

#include "val.h"

/* Types of heap-allocated objects. */
typedef enum cact_object_type {
	CACT_OBJ_PAIR,
	CACT_OBJ_STRING,
	CACT_OBJ_PROCEDURE,
	CACT_OBJ_ENVIRONMENT,
	CACT_OBJ_CONT,
	CACT_OBJ_ERROR,

	/* TODO: Ports */
	/* CACT_OBJ_PORT, */

	CACT_OBJ_VECTOR,

	/* TODO: Record types */
	/* CACT_OBJ_RECORD_TYPE, */
	/* CACT_OBJ_RECORD_INST, */

	/* TODO: Symbols*/
} cact_object_type_t;

const char * 
cact_obj_show_type(enum cact_object_type);

#include "store.h"

/* A value that has an associated heap component. */
typedef struct cact_object {
	cact_object_type_t type;
	cact_store_data_t store_data;
} cact_object_t;

ARRAY_DECL(cact_object_array, struct cact_object *);
typedef struct cact_object_array cact_object_array_t;

void 
cact_mark_object(cact_object_t *);

typedef void (*cact_object_mark_fn)(cact_object_t *);

void 
cact_destroy_object(cact_object_t *);

typedef void (*cact_object_destroy_fn)(cact_object_t *);

#define DEFINE_OBJECT_CONVERSION(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(cact_value_t x, char *extras) \
{ \
	if (x.type != CACT_TYPE_OBJ) { \
		fprintf(stderr, "%s: Expected object, but got %s.\n", extras, cact_value_show_type(x.type)); \
		abort(); \
	} \
	if (x.as.object->type != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, cact_obj_show_type(typemarker), cact_type_str(x)); \
		abort(); \
	} \
	return (returntype)x.as.object; \
}

#define DEFINE_OBJECT_CHECK(funcname, typemarker) \
static inline bool \
funcname(cact_value_t x) { \
	return x.type == CACT_TYPE_OBJ && x.as.object->type == typemarker; \
}

#endif // CACT_OBJ_H

