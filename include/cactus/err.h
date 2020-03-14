#ifndef __CACT_ERR_H__
#define __CACT_ERR_H__

#include "cactus/obj.h"
#include "cactus/val.h"

struct cact_error {
	struct cact_obj obj;
    char *msg;
    struct cact_val ctx;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_ERROR,       struct cact_error*,  cact_to_error,     err)
DEFINE_OBJECT_CHECK(cact_is_error, CACT_OBJ_ERROR)

struct cact_val cact_make_error(struct cactus *, char *msg, struct cact_val irr);

#endif // __CACT_ERR_H__

