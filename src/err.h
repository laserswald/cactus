#ifndef __CACT_ERR_H__
#define __CACT_ERR_H__

#include "storage/obj.h"
#include "val.h"

typedef struct cact_error {
	cact_object_t obj;
	char *msg;
	cact_value_t ctx;
} cact_error_t;

#define PROPAGATE_ERROR(err) if (cact_is_error(err)) return (err);

DEFINE_OBJECT_CONVERSION(CACT_OBJ_ERROR, cact_error_t*, cact_to_error, err)
DEFINE_OBJECT_CHECK(cact_is_error, CACT_OBJ_ERROR)

cact_value_t 
cact_make_error(cact_context_t *, char *, cact_value_t);

void 
cact_mark_error(cact_object_t *);

void 
cact_destroy_error(cact_object_t *);

/* Raise an exception. */
cact_value_t
cact_raise(cact_context_t *, cact_value_t);

#endif // __CACT_ERR_H__

