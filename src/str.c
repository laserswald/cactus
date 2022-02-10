#include "str.h"

#include "core.h"
#include "internal/xmalloc.h"

/* Create a string. */
struct cact_val
cact_make_string(struct cactus *cact, const char *str)
{
    struct cact_string *s;

    s = (struct cact_string *)cact_store_allocate(&cact->store, CACT_OBJ_STRING);
    s->str = xstrdup(str);

    return CACT_OBJ_VAL(s);
}

void
cact_mark_string(struct cact_obj *ignored)
{
    return;
}

void
cact_destroy_string(struct cact_obj *o)
{
    struct cact_string *s;

    s = (struct cact_string *) o;

    xfree(s->str);
}

