#ifndef CACT_PROC_H
#define CACT_PROC_H

#include "cactus/val.h"
#include "cactus/env.h"
#include "cactus/obj.h"

struct cactus;

typedef struct cact_val (*cact_native_func)(struct cactus *, struct cact_val);

/* A procedure object. */
struct cact_proc {
	struct cact_obj obj;
	struct cact_env *env;
	struct cact_val argl;
	struct cact_val body;
	cact_native_func nativefn;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PROCEDURE, struct cact_proc*, cact_to_procedure, proc)
DEFINE_OBJECT_CHECK(cact_is_procedure, CACT_OBJ_PROCEDURE)

/* Create a new user-defined procedure. */
struct cact_val cact_make_procedure(struct cactus *, struct cact_env *e,
                                    struct cact_val args, struct cact_val body);

/* Create a new native C procedure. */
struct cact_val cact_make_native_proc(struct cactus *, cact_native_func, int);

/* Apply the procedure with the given arguments. */
struct cact_val cact_proc_apply(struct cactus *, struct cact_proc *, struct cact_val);

void cact_mark_proc(struct cact_obj *);
void cact_destroy_proc(struct cact_obj *);


#endif // CACT_PROC_H

