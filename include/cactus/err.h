#ifndef __CACT_ERR_H__
#define __CACT_ERR_H__

#include "cactus/obj.h"
#include "cactus/val.h"

struct cact_error {
	struct cact_obj obj;
	char *msg;
	struct cact_val ctx;
};

#define PROPAGATE_ERROR(err) if (cact_is_error(err)) return (err);

DEFINE_OBJECT_CONVERSION(CACT_OBJ_ERROR,       struct cact_error*,  cact_to_error,     err)
DEFINE_OBJECT_CHECK(cact_is_error, CACT_OBJ_ERROR)

struct cact_val cact_make_error(struct cactus *, char *msg, struct cact_val irr);
void cact_mark_error(struct cact_obj *);
void cact_destroy_error(struct cact_obj *);

#endif // __CACT_ERR_H__

