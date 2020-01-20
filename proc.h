#ifndef CACT_PROC_H
#define CACT_PROC_H

#include "core.h"
#include "sexp.h"

typedef struct cact_val *(*cact_native_func)(struct cactus *, struct cact_val *);

struct cact_val *
cact_proc_apply(struct cactus *, struct cact_proc *, struct cact_val *args);

#endif // CACT_PROC_H

