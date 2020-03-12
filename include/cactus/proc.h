#ifndef CACT_PROC_H
#define CACT_PROC_H

#include "cactus/val.h"
#include "cactus/env.h"

struct cactus;

typedef struct cact_val (*cact_native_func)(struct cactus *, struct cact_val);

/* A procedure object. */
typedef struct cact_proc {
    struct cact_env *env;
    struct cact_val argl;
    struct cact_val body;
    cact_native_func nativefn;
} cact_proc;

struct cact_val cact_make_procedure(struct cactus *, cact_env *e, cact_val args, cact_val body);
struct cact_val cact_make_native_proc(struct cactus *, cact_native_func);

/* Apply the procedure with the given arguments. */
cact_val cact_proc_apply(struct cactus *, cact_proc*, cact_val);

#endif // CACT_PROC_H

