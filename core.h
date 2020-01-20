#ifndef __CACT_CORE_H__
#define __CACT_CORE_H__

#include "array.h"
#include "read.h"

/**
 * The core structure for a Cactus interpreter.
 */
struct cactus {
	struct cact_env *root_env;
	struct cact_env *current_env;
    struct cact_lexer lexer;
};

/* Initialize a cactus interpreter. */
void cact_init(struct cactus *);

/* Finalize a cactus interpreter. */
void cact_finish(struct cactus *);

/* Evaluate a file using the interpreter. */
struct cact_val * cact_eval_file(struct cactus *, FILE *);

/* Evaluate a string using the interpreter. */
struct cact_val * cact_eval_string(struct cactus *, char *);

void cact_push_frame(struct cactus *);
void cact_pop_frame(struct cactus *);
struct cact_stack_frame * cact_current_frame(struct cactus *);

#endif // __CACT_CORE_H__

