#ifndef CACT_OBJ_H
#define CACT_OBJ_H

#include "store.h"

/* Types of heap-allocated objects. */
enum cact_obj_type {
	CACT_OBJ_STRING,
	CACT_OBJ_PAIR,
	CACT_OBJ_PROCEDURE,
	CACT_OBJ_ENVIRONMENT,
	CACT_OBJ_ERROR,

    /* TODO: Ports */
    /* CACT_OBJ_PORT, */

    /* TODO: Vectors */
    /* CACT_OBJ_VECTOR, */

	/* TODO: Record types */
	/* CACT_OBJ_RECORD_TYPE, */ 
	/* CACT_OBJ_RECORD_INST, */
};

struct cact_string {
    char *str;
};

struct cact_error {
    char *msg;
    cact_val *ctx;
};

/* Fundamental data structure for heap-allocated objects. */
struct cact_obj {
    struct cact_store_data store_data;
    enum cact_obj_type type;
    union {
	    struct cact_string str;
	    struct cact_pair pair;
	    struct cact_proc proc;
	    struct cact_env env;
	    struct cact_error err;
    } as;
};

void cact_obj_mark(struct cact_obj *);
void cact_obj_destroy(struct cact_obj *);

#define DEFINE_OBJECT_CONVERSION(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(cact_val x, char *extras) \
{ \
	if (x.type != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, show_obj_type(typemarker), show_type(x->t)); \
		abort(); \
	} \
	return x.membername; \
}

#define DEFINE_OBJECT_CHECK(funcname, typemarker) \
static inline bool \
funcname(cact_val x) { \
	return x && x->t == CACT_TYPE_OBJECT && x->obj.type == typemarker; \
}

#endif // CACT_OBJ_H

