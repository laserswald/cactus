
#include "proc.h"

#include "core.h"
#include "val.h"
#include "pair.h"
#include "write.h"

#include "storage/store.h"
#include "evaluator/eval.h"
#include "internal/debug.h"
#include "internal/utils.h"

/* Create a procedure. */
struct cact_val
cact_make_procedure(struct cactus *cact, struct cact_env *e, struct cact_val argl, struct cact_val body)
{
    assert(cact);
    assert(e);

    struct cact_val envval = CACT_OBJ_VAL((struct cact_obj *)e);

    cact_preserve(cact, envval);
    cact_preserve(cact, argl);
    cact_preserve(cact, body);

    struct cact_proc *proc = (struct cact_proc *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
    proc->env = e;
    proc->argl = argl;
    proc->body = body;

    cact_unpreserve(cact, body);
    cact_unpreserve(cact, argl);
    cact_unpreserve(cact, envval);

    return CACT_OBJ_VAL((struct cact_obj *) proc);
}

struct cact_val
cact_make_native_proc(struct cactus *cact, cact_native_func fn)
{
    assert(cact);
    assert(fn != NULL);

    struct cact_proc *nat = (struct cact_proc *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
    nat->env = NULL;
    nat->nativefn = fn;

    return CACT_OBJ_VAL((struct cact_obj *) nat);
}

void
cact_mark_proc(struct cact_obj *o)
{
    assert(o);

    struct cact_proc *p = (struct cact_proc *) o;

    if (p->nativefn) {
        return;
    }

    if (p->env) {
        cact_obj_mark((struct cact_obj *) p->env);
    }

    cact_mark_val(p->argl);
    cact_mark_val(p->body);
}

void
cact_destroy_proc(struct cact_obj *o)
{
    return;
}


static void
cact_proc_eval_args(struct cactus *cact, struct cact_env *params_env,
                    struct cact_val params, struct cact_val args)
{
    assert(cact);
    assert(params_env);

    struct cact_val current_arg;

    current_arg = args;

    CACT_LIST_FOR_EACH_ITEM(cact, param, params) {
        if (cact_is_null(current_arg)) {
            die("eval_args: not enough arguments");
        }

        cact_eval_single(cact, cact_car(cact, current_arg));
        struct cact_val evaled_arg = cact_current_retval(cact);

        cact_env_define(
            cact,
            params_env,
            cact_to_symbol(param, "cact_proc_eval_args"),
            evaled_arg
        );

        current_arg = cact_cdr(cact, current_arg);
    }
}

/**
 * Apply a procedure given the arguments and the environment.
 */
struct cact_val
cact_proc_apply(struct cactus *cact, struct cact_proc *clo, struct cact_val args)
{
    assert(cact);
    assert(clo);
    assert(cact_is_pair(args) || cact_is_null(args));
    assert(clo->argl.as.object != args.as.object); // (in)sanity checking

    struct cact_cont *cc = cact_current_cont(cact);

    // Initialize the environment containing the parameters.
    cc->env = (struct cact_env *) cact_alloc(cact, CACT_OBJ_ENVIRONMENT);
    cact_env_init(cc->env, clo->env);

    // Set the arguments list.
    cc->argl = args;

    // Evaluate the arguments.
    cact_proc_eval_args(cact, cc->env, clo->argl, cc->argl);

    if (clo->nativefn != NULL) {
        return clo->nativefn(cact, args);
    } else {
        struct cact_val ret = cact_eval_list(cact, clo->body);
        return ret;
    }
}

