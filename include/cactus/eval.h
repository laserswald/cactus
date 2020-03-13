#ifndef EVAL_H
#define EVAL_H

#include "core.h"
#include "pair.h"

struct cact_val special_define(struct cactus *, struct cact_val);
struct cact_val special_if(struct cactus *, struct cact_val);
struct cact_val special_begin(struct cactus *, struct cact_val);
struct cact_val special_set_bang(struct cactus *, struct cact_val);
struct cact_val special_lambda(struct cactus *, struct cact_val);
struct cact_val special_quote(struct cactus *, struct cact_val);

struct cact_val cact_eval(struct cactus *, struct cact_val);

/* Evaluate a file using the interpreter. */
struct cact_val cact_eval_file(struct cactus *, FILE *);

/* Evaluate a string using the interpreter. */
struct cact_val cact_eval_string(struct cactus *, char *);

/* Evaluate a list of expressions, and return the value of the last expression. */
struct cact_val cact_eval_list(struct cactus *, struct cact_val);

#endif // cact_eval_h_INCLUDED

