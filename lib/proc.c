
#include "cactus/proc.h"

#include "cactus/core.h"
#include "cactus/val.h"
#include "cactus/pair.h"
#include "cactus/store.h"
#include "cactus/write.h"
#include "cactus/eval.h"

/* Create a procedure. */
struct cact_val
cact_make_procedure(struct cactus *cact, struct cact_env *e, struct cact_val argl, struct cact_val body)
{
    struct cact_val proc;
    proc.type = CACT_TYPE_OBJ;
    proc.as.object = cact_store_allocate(&cact->store);
    proc.as.object->type = CACT_OBJ_PROCEDURE;
    proc.as.object->as.proc.env = e;
    proc.as.object->as.proc.argl = argl;
    proc.as.object->as.proc.body = body;
    return proc;
}

/* Apply a procedure given the arguments and the environment. */
struct cact_val
cact_proc_apply(struct cactus *cact, struct cact_proc *clo, struct cact_val args)
{
    if (clo->nativefn) {
        return clo->nativefn(cact, args);
    } else {
        struct cact_env *params_env = malloc(sizeof(struct cact_env));
        cact_env_init(params_env, clo->env);

        struct cact_val current_arg = args;
        CACT_LIST_FOR_EACH_ITEM(cact, param, clo->argl) {
            if (cact_is_null(current_arg)) {
                fprintf(stderr, "Not enough arguments!");
                fprint_sexp(stderr, args);
                abort();
            }
            cact_env_define(cact, params_env, param, cact_eval(cact, cact_car(cact, current_arg)));
            current_arg = cact_cdr(cact, current_arg);
        }
        cact->current_env = params_env;
        struct cact_val ret = cact_eval_list(cact, clo->body);
        cact->current_env = cact->current_env->parent;
        free(params_env);
        return ret;
    }
}

struct cact_val
cact_make_native_proc(struct cactus *cact, cact_native_func fn)
{
    struct cact_val nat;
    nat.type = CACT_TYPE_OBJ;
    nat.as.object = cact_store_allocate(&cact->store);
    nat.as.object->as.proc.nativefn = fn;
    return nat;
}

