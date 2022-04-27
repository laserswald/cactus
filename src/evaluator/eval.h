#ifndef CACT_EVAL_H
#define CACT_EVAL_H

#include "core.h"
#include "pair.h"

// Entry points from the continuation 
void cact_eval_prim(cact_context_t *, cact_frame_t *cc);
void cact_eval_branch(cact_context_t *, cact_frame_t *cc);
void cact_eval_assignment(cact_context_t *, cact_frame_t *cc);
void cact_eval_definition(cact_context_t *, cact_frame_t *cc);
void cact_eval_sequence(cact_context_t *, cact_frame_t *cc);
void cact_eval_apply_with_operator(cact_context_t *, cact_frame_t *cc);
void cact_eval_arg_pop(cact_context_t *, cact_frame_t *cc);
void cact_eval_arg_bind(cact_context_t *, cact_frame_t *cc);
void cact_eval_extend_env(cact_context_t *, cact_frame_t *cc);
void cact_eval_apply(cact_context_t *, cact_frame_t *cc);

/* Evaluate a single expression using the interpreter. */
cact_value_t cact_eval_single(cact_context_t *, cact_value_t);

/* Evaluate a file using the interpreter. */
cact_value_t cact_eval_file(cact_context_t *, FILE *);

/* Evaluate a string using the interpreter. */
cact_value_t cact_eval_string(cact_context_t *, char *);

/* Evaluate a list of expressions, and return the value of the last expression. */
cact_value_t cact_eval_list(cact_context_t *, cact_value_t);

#endif // CACT_EVAL_H
