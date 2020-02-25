#ifndef CACT_PROC_H
#define CACT_PROC_H

#include "core.h"
#include "sexp.h"
#include "obj.h"
#include "env.h"

typedef struct cact_val *(*cact_native_func)(struct cactus *, struct cact_val *);

/* A procedure object. */
struct cact_proc {
	struct cact_obj obj;
    struct cact_env *env;
    struct cact_val *argl;
    struct cact_val *body;
    cact_native_func nativefn;
};

/* Apply the procedure with the given arguments. */
struct cact_val *cact_proc_apply(struct cactus *, struct cact_proc *, struct cact_val *args);

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PROCEDURE, struct cact_proc *, cact_to_procedure, c)
DEFINE_OBJECT_CHECK(cact_val_is_proc, CACT_OBJ_PROCEDURE)

#endif // CACT_PROC_H

