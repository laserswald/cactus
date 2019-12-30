#include "sexp.h"
#include "eval.h"
#include "builtin.h"
#include "write.h"
#include "env.h"

/* Apply a procedure given the arguments and the environment. */
cact_val *
cact_proc_apply(cact_proc *clo, cact_val *args, cact_env *e)
{
    if (clo->nativefn) {
        return clo->nativefn(args, e);
    } else {
        cact_env params_env = {0};
        envinit(&params_env, clo->env);
        cact_val *current_arg = args;
        LIST_FOR_EACH(clo->argl, param) {
            if (! current_arg) {
                fprintf(stderr, "Not enough arguments!");
                fprint_sexp(stderr, args);
                abort();
            }
            envadd(&params_env, car(param), cact_eval(car(current_arg), e));
            current_arg = cdr(current_arg);
        }
        return cact_builtin_begin(clo->body, &params_env);
    }
}
