#include "cactus/cont.h"

#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/obj.h"

void
cact_cont_init(struct cact_cont *cont, struct cact_env *frame, struct cact_proc *exnh)
{
    assert(cont);
    assert(frame);

    cont->env = frame;
    cont->exn_handler = exnh;
}

/* Mark a continuation to be protected from garbage collection. */
void
cact_mark_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_mark((struct cact_obj *)c->env);
}

/* Destroy any data that this continuation points to. */
void
cact_destroy_cont(struct cact_obj *o)
{
    assert(o);

    struct cact_cont *c = (struct cact_cont *) o;

    cact_obj_destroy((struct cact_obj *)c->env);
}

