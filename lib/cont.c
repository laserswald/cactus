#include <setjmp.h>

#include "cactus/cont.h"

#include "cactus/core.h"
#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/obj.h"
#include "cactus/eval.h"

#include "cactus/internal/utils.h"

void
cact_cont_init(struct cact_cont *cont, struct cact_env *env, struct cact_proc *proc)
{
    assert(cont);
    assert(proc);

    cont->env = env;
    cont->exn_handler = NULL;

    cont->proc = proc;
    cont->argl = CACT_UNDEF_VAL;
    cont->evlist = CACT_UNDEF_VAL;
    cont->retval = CACT_UNDEF_VAL;
}

/* Mark a continuation to be protected from garbage collection. */
void
cact_mark_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_mark((struct cact_obj *)c->env);
    cact_obj_mark((struct cact_obj *)c->exn_handler);
    cact_obj_mark((struct cact_obj *)c->proc);

    cact_mark_val(c->argl);
    cact_mark_val(c->evlist);
    cact_mark_val(c->retval);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_destroy((struct cact_obj *)c->env);
}

/**
 * Resume the continuation. Note that continuations need not be paused in order to resume them.
 */
struct cact_val
cact_resume_cont(struct cactus *cact, struct cact_cont *cont)
{
	struct cact_val retval;

    while (! cact_is_null(cont->evlist)) {

        switch (setjmp(cont->bounce)) {
        case CACT_JMP_INITIAL:
            cact_eval_prim(cact, cact_car(cact, cont->evlist), true);
            break;

        case CACT_JMP_CALL:
            cact_proc_apply(cact, cont->proc, cont->argl);
            break;

        case CACT_JMP_RETURN:
	        retval = cont->retval;
            cact_call_stack_pop(cact);
            break;

        default:
            die("cact_cont_start: got mystery jump return value");
        }
    }

    return retval;
}

/* Jump back to the current trampoline, but without adding more stuff to do. */
void
cact_return(struct cactus *cact, struct cact_val value)
{
	struct cact_cont *cont = cact_current_cont(cact);

	cont->retval = value;

	longjmp(cont->bounce, CACT_JMP_RETURN);

	return;
}

/* 
 * Jump back to the trampoline, but load the current continuation frame with 
 * what we need to do next. 
 */
void
cact_tailcall(struct cactus *cact, struct cact_proc *proc, struct cact_val args)
{
	struct cact_cont *cont = cact_current_cont(cact);

	cont->proc = proc;
	cont->evlist = CACT_NULL_VAL;
	cont->argl = args;

	longjmp(cont->bounce, CACT_JMP_CALL);

	return;
}

void
cact_call(struct cactus *cact, struct cact_proc *proc, struct cact_val args)
{
    struct cact_cont *nc;

    assert(cact);

    nc = (struct cact_cont *) cact_alloc(cact, CACT_OBJ_CONT);
    cact_cont_init(nc, cact_current_env(cact), proc);

    SLIST_INSERT_HEAD(&cact->conts, nc, parent);

    cact_tailcall(cact, proc, args);
}

