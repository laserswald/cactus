#ifndef CACT_OBJ_H
#define CACT_OBJ_H

#include "val.h"

/* Types of heap-allocated objects. */
enum cact_obj_type {
	CACT_OBJ_PAIR,
	CACT_OBJ_STRING,
	CACT_OBJ_PROCEDURE,
	CACT_OBJ_ENVIRONMENT,
	CACT_OBJ_CONT,
	CACT_OBJ_ERROR,
	CACT_OBJ_PORT,
	CACT_OBJ_VECTOR,

	/* TODO: Record types */
	/* CACT_OBJ_RECORD_TYPE, */
	/* CACT_OBJ_RECORD_INST, */

	/* TODO: Symbols*/
};

#include "store.h"

/* A value that has an associated heap component. */
struct cact_obj {
	enum cact_obj_type type;
	struct cact_store_data store_data;
};

void cact_obj_mark(struct cact_obj *);
void cact_obj_destroy(struct cact_obj *);

const char* cact_obj_show_type(enum cact_obj_type);

#define DEFINE_OBJECT_CONVERSION(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(struct cact_val x, char *extras) \
{ \
	if (x.type != CACT_TYPE_OBJ) { \
		fprintf(stderr, "%s: Expected object, but got %s.\n", extras, cact_val_show_type(x.type)); \
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
funcname(struct cact_val x) { \
	return x.type == CACT_TYPE_OBJ && x.as.object->type == typemarker; \
}

#endif // CACT_OBJ_H

