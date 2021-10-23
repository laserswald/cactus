#ifndef CACT_EVAL_H
#define CACT_EVAL_H

#include "core.h"
#include "pair.h"

/* Evaluate a single expression using the interpreter. */
void cact_eval_prim(struct cactus *, struct cact_val, bool tail);

/* Evaluate a file using the interpreter. */
struct cact_val cact_eval_file(struct cactus *, FILE *);

/* Evaluate a string using the interpreter. */
struct cact_val cact_eval_string(struct cactus *, char *);

/* Evaluate a list of expressions, and return the value of the last expression. */
struct cact_val cact_eval_list(struct cactus *, struct cact_val);

#endif // CACT_EVAL_H
