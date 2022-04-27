
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
cact_value_t
cact_make_procedure(cact_context_t *cact, cact_env_t *e, cact_value_t argl, cact_value_t body)
{
    assert(cact);
    assert(e);

    cact_value_t envval = CACT_OBJ_VAL((cact_object_t *)e);

    cact_preserve(cact, envval);
    cact_preserve(cact, argl);
    cact_preserve(cact, body);

    cact_procedure_t *proc = (cact_procedure_t *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
    proc->env = e;
    proc->argl = argl;
    proc->body = body;

    cact_unpreserve(cact, body);
    cact_unpreserve(cact, argl);
    cact_unpreserve(cact, envval);

    return CACT_OBJ_VAL((cact_object_t *) proc);
}

cact_value_t
cact_make_native_proc(cact_context_t *cact, cact_native_func_t fn)
{
    assert(cact);
    assert(fn != NULL);

    cact_procedure_t *nat = (cact_procedure_t *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
    nat->env = NULL;
    nat->nativefn = fn;

    return CACT_OBJ_VAL((cact_object_t *) nat);
}

void
cact_mark_proc(cact_object_t *o)
{
    assert(o);

    cact_procedure_t *p = (cact_procedure_t *) o;

    if (p->nativefn) {
        return;
    }

    if (p->env) {
        cact_mark_object((cact_object_t *) p->env);
    }

    cact_mark_value(p->argl);
    cact_mark_value(p->body);
}

void
cact_destroy_proc(cact_object_t *o)
{
    return;
}


static void
cact_proc_eval_args(cact_context_t *cact, cact_env_t *params_env,
                    cact_value_t params, cact_value_t args)
{
    assert(cact);
    assert(params_env);

    cact_value_t current_arg;

    current_arg = args;

    CACT_LIST_FOR_EACH_ITEM(cact, param, params) {
        if (cact_is_null(current_arg)) {
            die("eval_args: not enough arguments");
        }

        cact_eval_single(cact, cact_car(cact, current_arg));
        cact_value_t evaled_arg = cact_current_retval(cact);

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
cact_value_t
cact_proc_apply(cact_context_t *cact, cact_procedure_t *clo, cact_value_t args)
{
    assert(cact);
    assert(clo);
    assert(cact_is_pair(args) || cact_is_null(args));
    assert(clo->argl.as.object != args.as.object); // (in)sanity checking

    cact_frame_t *cc = cact_current_frame(cact);

    // Initialize the environment containing the parameters.
    cc->env = (cact_env_t *) cact_alloc(cact, CACT_OBJ_ENVIRONMENT);
    cact_env_init(cc->env, clo->env);

    // Set the arguments list.
    cc->argl = args;

    // Evaluate the arguments.
    cact_proc_eval_args(cact, cc->env, clo->argl, cc->argl);

    if (clo->nativefn != NULL) {
        return clo->nativefn(cact, args);
    } else {
        cact_value_t ret = cact_eval_list(cact, clo->body);
        return ret;
    }
}

