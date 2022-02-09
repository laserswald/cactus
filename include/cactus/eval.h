#ifndef CACT_EVAL_H
#define CACT_EVAL_H

#include "core.h"
#include "pair.h"

// Entry points from the continuation 
void cact_eval_prim(struct cactus *, struct cact_cont *cc);
void cact_eval_branch(struct cactus *, struct cact_cont *cc);
void cact_eval_assignment(struct cactus *, struct cact_cont *cc);
void cact_eval_definition(struct cactus *, struct cact_cont *cc);
void cact_eval_sequence(struct cactus *, struct cact_cont *cc);
void cact_eval_apply_with_operator(struct cactus *, struct cact_cont *cc);
void cact_eval_arg_pop(struct cactus *, struct cact_cont *cc);
void cact_eval_arg_bind(struct cactus *, struct cact_cont *cc);
void cact_eval_extend_env(struct cactus *, struct cact_cont *cc);
void cact_eval_apply(struct cactus *, struct cact_cont *cc);

/* Evaluate a single expression using the interpreter. */
struct cact_val cact_eval_single(struct cactus *, struct cact_val);

/* Evaluate a file using the interpreter. */
struct cact_val cact_eval_file(struct cactus *, FILE *);

/* Evaluate a string using the interpreter. */
struct cact_val cact_eval_string(struct cactus *, char *);

/* Evaluate a list of expressions, and return the value of the last expression. */
struct cact_val cact_eval_list(struct cactus *, struct cact_val);

#endif // CACT_EVAL_H
