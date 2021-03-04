
#include "cactus/proc.h"

#include "cactus/core.h"
#include "cactus/val.h"
#include "cactus/pair.h"
#include "cactus/store.h"
#include "cactus/write.h"
#include "cactus/eval.h"
#include "cactus/internal/debug.h"

/* Create a procedure. */
struct cact_val
cact_make_procedure(struct cactus *cact, struct cact_env *e, 
        struct cact_val argl, struct cact_val body)
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
cact_make_native_proc(struct cactus *cact, cact_native_func fn, int arity)
{
    assert(cact);
    assert(fn != NULL);

    struct cact_proc *nat = (struct cact_proc *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
    nat->nativefn = fn;
    nat->argl = CACT_NULL_VAL;

    return CACT_OBJ_VAL((struct cact_obj *) nat);
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
            fprintf(stderr, "Not enough arguments!");
            cact_fdisplay(stderr, args);
            abort();
        }

        struct cact_val evaled_arg = cact_eval(cact, cact_car(cact, current_arg));

        cact_env_define(
            cact,
            params_env,
            cact_to_symbol(param, "cact_proc_eval_args"),
            evaled_arg
        );

        current_arg = cact_cdr(cact, current_arg);
    }
}

/* Apply a procedure given the arguments and the environment. */
struct cact_val
cact_proc_apply(struct cactus *cact, struct cact_proc *clo, struct cact_val args)
{
    assert(cact);
    assert(clo);
    assert(cact_is_pair(args) || cact_is_null(args));
    assert(clo->argl.as.object != args.as.object); // (in)sanity checking

    struct cact_env *params_env;
    params_env = (struct cact_env *) cact_alloc(cact, CACT_OBJ_ENVIRONMENT);
    cact_env_init(params_env, clo->env);
    cact_preserve(cact, CACT_OBJ_VAL(params_env));

    if (clo->nativefn == NULL) {
        cact_proc_eval_args(cact, params_env, clo->argl, args);
    }

    cact_call_stack_push(cact, params_env);
    cact_unpreserve(cact, CACT_OBJ_VAL(params_env)); // params_env

    struct cact_val ret;
    if (clo->nativefn != NULL) {
        // TODO: evaluate args then pass the list
        ret = clo->nativefn(cact, args);
    } else {
        ret = cact_eval_list(cact, clo->body);
    }

    cact_call_stack_pop(cact);

    return ret;
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

