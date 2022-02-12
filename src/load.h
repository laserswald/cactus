#ifndef __CACTUS_LOAD_H__
#define __CACTUS_LOAD_H__

#include "core.h"

#include <stdio.h>

/* Evaluate a file using the interpreter. */
struct cact_val
cact_eval_file(struct cactus *cact, FILE *in);

/* Evaluate a string using the interpreter. */
struct cact_val
cact_eval_string(struct cactus *cact, char *s);

struct cact_val
cact_builtin_load(struct cactus *cact, struct cact_val x);

#endif // __CACTUS_LOAD_H__

