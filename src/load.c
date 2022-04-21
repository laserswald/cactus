#include "load.h"

#include "internal/utils.h"
#include "write.h"
#include "evaluator/eval.h"
#include "str.h"
#include "err.h"

#include <stdio.h>
#include <stdlib.h>

/* Evaluate a file using the interpreter. */
struct cact_val
cact_eval_file(struct cactus *cact, FILE *in)
{
    return cact_eval_string(cact, slurp(in));
}

/* Evaluate a string using the interpreter. */
struct cact_val
cact_eval_string(struct cactus *cact, char *s)
{
    struct cact_cont *cc = cact_current_cont(cact);
    cact_cont_init_load(cc, cact_current_env(cact), s);
    cact_cont_start(cact, cc);
    return cc->retval;
}

