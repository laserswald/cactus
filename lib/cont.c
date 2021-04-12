#include <setjmp.h>

#include "cactus/cont.h"

#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/obj.h"

void
cact_cont_init(struct cact_cont *cont, struct cact_proc *proc)
{
    assert(cont);
    assert(frame);

    cont->env = NULL;
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
    cact_obj_mark((struct cact_obj *)c->argl);
    cact_obj_mark((struct cact_obj *)c->evlist);
    cact_obj_mark((struct cact_obj *)c->retval);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_destroy((struct cact_obj *)c->env);
}

/* Jump back to the current trampoline, but without adding more stuff to do. */
void
cact_return(struct cactus *cact, struct cact_val value)
{
	struct cact_cont *current_cont = cact_current_continuation(cact);

	cont->retval = value;

	longjmp(cont->bounce, CACT_JMP_RETURN);

	return;
}

/* Jump back to the trampoline, but replace the return value of that trampoline with 
 * the return value of */
void
cact_tailcall(struct cactus *cact, struct cact_proc *proc, struct cact_val args)
{
	struct cact_cont *current_cont = cact_current_continuation(cact);

	cont->proc = proc;
	cont->evlist = CACT_NULL_VAL;
	cont->argl = args;

	longjmp(cont->bounce, CACT_JMP_TCALL);

	return;
}
