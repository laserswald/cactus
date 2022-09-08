#ifndef CACT_PROC_H
#define CACT_PROC_H

#include "val.h"
#include "env.h"
#include "storage/obj.h"

typedef cact_value_t (*cact_native_func_t)(cact_context_t *, cact_value_t);

/* A procedure object. */
typedef struct cact_procedure {
	cact_object_t obj;
	cact_env_t *env;
	cact_value_t argl;
	cact_value_t body;
	cact_native_func_t nativefn;
} cact_procedure_t;

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PROCEDURE, cact_procedure_t *, cact_to_procedure, proc)
DEFINE_OBJECT_CHECK(cact_is_procedure, CACT_OBJ_PROCEDURE)

/* Create a new user-defined procedure. */
cact_value_t cact_make_procedure(cact_context_t *, cact_env_t *e, cact_value_t args, cact_value_t body);

/* Create a new native C procedure. */
cact_value_t cact_make_native_proc(cact_context_t *, cact_native_func_t);

/* Apply the procedure with the given arguments. */
cact_value_t cact_proc_apply(cact_context_t *, cact_procedure_t *, cact_value_t);

void cact_mark_proc(cact_object_t *);
void cact_destroy_proc(cact_object_t *);

#endif // CACT_PROC_H

