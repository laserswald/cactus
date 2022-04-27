#include "str.h"

#include "core.h"
#include "internal/xmalloc.h"

/* Create a string. */
cact_value_t
cact_make_string(cact_context_t *cact, const char *str)
{
    cact_string_t *s;

    s = (cact_string_t *)cact_store_allocate(&cact->store, CACT_OBJ_STRING);
    s->str = xstrdup(str);

    return CACT_OBJ_VAL(s);
}

void
cact_mark_string(cact_object_t *ignored)
{
    return;
}

void
cact_destroy_string(cact_object_t *o)
{
    cact_string_t *s;

    s = (cact_string_t *) o;

    xfree(s->str);
}

