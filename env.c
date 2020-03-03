#include <stdlib.h>
#include "write.h"
#include "sexp.h"
#include "env.h"
#include "store.h"
#include "core.h"

/* Create an environment. */
struct cact_val
cact_make_env(struct cactus *cact, cact_env *parent)
{
	cact_val v;
	v.type = CACT_TYPE_OBJ;
	v.as.object = cact_store_allocate(&cact->store);
	cact_env_init(&v.as.object->as.env, parent);
    return v;
}

void
cact_env_init(struct cact_env *e, struct cact_env *parent)
{
    e->parent = parent;
}

/* 
 * Look up the key in the environment and return it, or raise an error
 */
struct cact_val
cact_env_lookup(struct cact_env *e, struct cact_val key)
{
    if (!e) 
	    return CACT_NULL_VAL;

    struct cact_val found_kv = cact_assoc(key, e->list);

    if (cact_is_null(found_kv) && e->parent) {
        found_kv = cact_env_lookup(e->parent, key);
    }

    if (cact_is_null(found_kv)) {
        return cact_make_error("Symbol not defined", key);
    }

    return found_kv;
}

/*
 * Perform an environment definition, as opposed to an environment
 * assignment.
 */
int 
cact_env_define(struct cactus *cact, cact_env *e, cact_val key, cact_val val)
{
    if (!e)
        return -1;

    cact_val found_kv = cact_assoc(key, e->list);

    if (cact_is_null(found_kv)) {
        return -2;
    }

    e->list = cact_acons(cact, key, val, e->list);

    return 0;
}

/* Assign the key to the value, ensuring the key already exists. */
int 
cact_env_set(cact_env *e, cact_val key, cact_val val)
{
    if (!e)
        return -1;

    cact_val found_kv = cact_env_lookup(e, key);

    if (cact_is_null(found_kv)) return -2;

    cact_set_cdr(found_kv, val);

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

