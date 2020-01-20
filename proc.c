#include "sexp.h"
#include "eval.h"
#include "builtin.h"
#include "write.h"
#include "env.h"

/* Apply a procedure given the arguments and the environment. */
struct cact_val *
cact_proc_apply(struct cactus *cact, cact_proc *clo, cact_val *args)
{
    if (clo->nativefn) {
        return clo->nativefn(cact, args);
    } else {
        struct cact_env *params_env = malloc(sizeof(struct cact_env));
        envinit(params_env, clo->env);
        cact_val *current_arg = args;
        LIST_FOR_EACH(clo->argl, param) {
            if (! current_arg) {
                fprintf(stderr, "Not enough arguments!");
                fprint_sexp(stderr, args);
                abort();
            }
            envadd(params_env, car(param), cact_eval(cact, car(current_arg)));
            current_arg = cdr(current_arg);
        }
        cact->current_env = params_env;
        struct cact_val *ret = cact_builtin_begin(cact, clo->body);
        cact->current_env = cact->current_env->parent;
        free(params_env);
        return ret;
    }
}
