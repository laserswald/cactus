#ifndef CACT_OBJ_H
#define CACT_OBJ_H

#include "proc.h"
#include "str.h"
#include "sym.h"
#include "pair.h"
#include "env.h"
#include "err.h"
#include "store.h"

struct cact_store_data;

/* Types of heap-allocated objects. */
enum cact_obj_type {
	CACT_OBJ_PAIR,
	CACT_OBJ_STRING,
	CACT_OBJ_SYMBOL,
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

	/* TODO: Symbols*/
}i;


struct cact_error {
    char *msg;
    struct cact_val ctx;
};

/* A value that has an associated heap component. */
struct cact_obj {
    struct cact_store_data store_data;
    enum cact_obj_type type;
    union {
	    struct cact_pair pair;
	    struct cact_string str;
	    struct cact_symbol sym;
	    struct cact_proc proc;
	    struct cact_env env;
	    struct cact_error err;
    } as;
};

void cact_obj_mark(struct cact_obj *);
void cact_obj_destroy(struct cact_obj *);

const char* cact_obj_show_type(enum cact_obj_type);

#define DEFINE_OBJECT_CONVERSION(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(cact_val x, char *extras) \
{ \
	if (x.type != CACT_TYPE_OBJ) { \
		fprintf(stderr, "%s: Expected object, but got %s.\n", extras, cact_val_show_type(x.type)); \
		abort(); \
	} \
	if (x.as.object->type != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, cact_obj_show_type(typemarker), cact_type_str(x)); \
		abort(); \
	} \
	return x.as.object->as.membername; \
}

#define DEFINE_OBJECT_CHECK(funcname, typemarker) \
static inline bool \
funcname(cact_val x) { \
	return x.type == CACT_TYPE_OBJ && x.as.object->type == typemarker; \
}

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PAIR,        struct cact_pair,   cact_to_pair,      pair)
DEFINE_OBJECT_CONVERSION(CACT_OBJ_STRING,      struct cact_string, cact_to_string,    str)
DEFINE_OBJECT_CONVERSION(CACT_OBJ_SYMBOL,      struct cact_symbol, cact_to_sym,       sym)
DEFINE_OBJECT_CONVERSION(CACT_OBJ_PROCEDURE,   struct cact_proc,   cact_to_procedure, proc)
DEFINE_OBJECT_CONVERSION(CACT_OBJ_ENVIRONMENT, struct cact_env,    cact_to_env,       env)
DEFINE_OBJECT_CONVERSION(CACT_OBJ_ERROR,       struct cact_error,  cact_to_error,     err)

DEFINE_OBJECT_CHECK(cact_is_pair, CACT_OBJ_PAIR)
DEFINE_OBJECT_CHECK(cact_is_string, CACT_OBJ_STRING)
DEFINE_OBJECT_CHECK(cact_is_symbol, CACT_OBJ_SYMBOL)
DEFINE_OBJECT_CHECK(cact_is_procedure, CACT_OBJ_PROCEDURE)
DEFINE_OBJECT_CHECK(cact_is_env, CACT_OBJ_ENVIRONMENT)
DEFINE_OBJECT_CHECK(cact_is_error, CACT_OBJ_ERROR)
#endif // CACT_OBJ_H

