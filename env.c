#include <stdlib.h>
#include "sexp.h"
#include "env.h"

void
envinit(cact_env *e, cact_env *parent)
{
    e->parent = parent;
}

/* 
 * Look up the key in the environment and return the associated sexp
 * or null. 
 */
cact_val *
envlookup(cact_env *e, cact_val *key)
{
    if (!e) return NULL;

    cact_val *found_kv = assoc(key, e->list);

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
int 
envadd(cact_env *e, cact_val *key, cact_val *val)
{
    if (!e)
        return -1;

    cact_val *found_kv = assoc(key, e->list);

    if (found_kv)
        return -2;

    e->list = acons(key, val, e->list);

    return 0;
}

/* Assign the key to the value, ensuring the key already exists. */
int 
envset(cact_env *e, cact_val *key, cact_val *val)
{
    if (!e)
        return -1;

    cact_val *found_kv = envlookup(e, key);

    if (! found_kv)
        return -2;

    found_kv->p.cdr = val;

    return 0;
}

void
print_env(cact_env *e)
{
    if (!e)
        return;

    printf("cact_environment:");
    print_list(e->list);
    puts("");
    puts("Parent:");
    print_env(e->parent);
}

