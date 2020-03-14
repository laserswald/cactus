
#include <stdlib.h>

#include "cactus/env.h"

#include "cactus/write.h"
#include "cactus/val.h"
#include "cactus/store.h"
#include "cactus/core.h"
#include "cactus/pair.h"

/* Create an environment. */
struct cact_val
cact_make_env(struct cactus *cact, struct cact_env *parent)
{
	struct cact_env *env = (struct cact_env *)cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
	cact_env_init(env, parent);
    return CACT_OBJ_VAL((struct cact_obj *) env);
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
cact_env_lookup(struct cactus *cact, struct cact_env *e, struct cact_val key)
{
    if (!e) 
	    return CACT_NULL_VAL;

    struct cact_val found_kv = cact_assoc(cact, key, e->list);

    if (cact_is_null(found_kv) && e->parent) {
        found_kv = cact_env_lookup(cact, e->parent, key);
    }

    if (cact_is_null(found_kv)) {
        return cact_make_error(cact, "Symbol not defined", key);
    }

    return found_kv;
}

/*
 * Perform an environment definition, as opposed to an environment
 * assignment.
 */
int 
cact_env_define(struct cactus *cact, struct cact_env *e, struct cact_val key, struct cact_val val)
{
    if (!e)
        return -1;

    struct cact_val found_kv = cact_assoc(cact, key, e->list);

    if (cact_is_null(found_kv)) {
        return -2;
    }

    e->list = cact_list_acons(cact, key, val, e->list);

    return 0;
}

/* Assign the key to the value, ensuring the key already exists. */
int 
cact_env_set(struct cactus *cact, struct cact_env *e, struct cact_val key, struct cact_val val)
{
    if (!e)
        return -1;

    struct cact_val found_kv = cact_env_lookup(cact, e, key);

    if (cact_is_null(found_kv)) return -2;

    cact_set_cdr(cact, found_kv, val);

    return 0;
}

void
print_env(struct cact_env *e)
{
    if (!e)
        return;

    printf("cact_environment:");
    print_list(e->list);
    puts("");
    puts("Parent:");
    print_env(e->parent);
}

