#include <stdlib.h>
#include "sexp.h"
#include "env.h"

void
envinit(Env *e, Env *parent)
{
    e->parent = parent;
}

Sexp *
envlookup(Env *e, Sexp *key)
{
    if (!e) return NULL;

    Sexp *found_kv = assoc(key, e->list);

    if (!found_kv && e->parent) {
        found_kv = envlookup(e->parent, key);
    }

    if (!found_kv) {
        return NULL;
    }

    return found_kv;
}

/*
 * Perform an environment definition, as opposed to an environment
 * assignment.
 */
int envadd(Env *e, Sexp *key, Sexp *val)
{
    if (!e)
        return -1;

    Sexp *found_kv = assoc(key, e->list);

    if (found_kv)
        return -2;

    e->list = acons(key, val, e->list);

    return 0;
}

int envset(Env *e, Sexp *key, Sexp *val)
{
    if (!e)
        return -1;

    Sexp *found_kv = envlookup(e, key);

    if (! found_kv)
        return -2;

    found_kv->p.cdr = val;

    return 0;
}

void
print_env(Env *e)
{
    if (!e)
        return;

    printf("Environment:");
    print_list(e->list);
    puts("");
    puts("Parent:");
    print_env(e->parent);
}

