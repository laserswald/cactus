
#include <stdlib.h>

#include "cactus/core.h"
#include "cactus/store.h"
#include "cactus/env.h"
#include "cactus/val.h"
#include "cactus/sym.h"
#include "cactus/write.h"
#include "cactus/err.h"

#include "cactus/internal/utils.h"
#include "cactus/internal/table.h"
#include "cactus/internal/debug.h"

TABLE_GENERATE(cact_env_entries, struct cact_symbol *, struct cact_val)

/*
 * Create and initialize a new environment.
 */
struct cact_val
cact_make_env(struct cactus *cact, struct cact_env *parent)
{
    struct cact_env *env = (struct cact_env *)cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
    cact_env_init(env, parent);
    return CACT_OBJ_VAL((struct cact_obj *) env);
}

/*
 * Initialize a given environment to have no entries and set it's parent.
 */
void
cact_env_init(struct cact_env *e, struct cact_env *parent)
{
    assert(e != parent);

    e->parent = parent;
    TABLE_INIT(&e->entries, ptrhash, cact_symbol_cmp);
}

/*
 * Mark the environment as reachable during garbage collection.
 */
void
cact_mark_env(struct cact_obj *o)
{
    struct cact_env *e;
    struct TABLE_ENTRY(cact_env_entries) *bucket;

    e = (struct cact_env *) o;

    TABLE_FOREACH_BUCKET(, bucket, &e->entries) {
        if (bucket->state == TABLE_ENTRY_FILLED) {
            cact_mark_val(bucket->val);
        }
    }

    if (e->parent) {
        cact_mark_env((struct cact_obj *) e->parent);
    }
}

/*
 * Destroy any data contained within this environment.
 */
void
cact_destroy_env(struct cact_obj *o)
{
    struct cact_env *e;

    e = (struct cact_env *) o;

    TABLE_CLEAR(&e->entries);
}

/*
 * Look up the key in the environment and return it, or raise an error.
 */
struct cact_val
cact_env_lookup(struct cactus *cact, struct cact_env *e, struct cact_symbol *key)
{
    assert(e);
    assert(key);

    struct cact_val *found = TABLE_FIND(cact_env_entries, &e->entries, key);

    if (! found) {
        if (e->parent) {
            return cact_env_lookup(cact, e->parent, key);
        } else {
            return cact_make_error(cact, "env lookup: no variable with key", CACT_SYM_VAL(key));
        }
    }

    return *found;
}

/*
 * Perform an environment definition, as opposed to an environment
 * assignment.
 */
struct cact_val
cact_env_define(struct cactus *cact, struct cact_env *e,
                struct cact_symbol *key, struct cact_val val)
{
    assert(e);
    assert(key);

    if (TABLE_HAS(cact_env_entries, &e->entries, key)) {
        fprintf(stderr, "Warning: redefining variable %s\n", key->sym);
    }

    TABLE_ENTER(cact_env_entries, &e->entries, key, val);

    return CACT_UNDEF_VAL;
}

/* Assign the key to the value, ensuring the key already exists. */
struct cact_val
cact_env_set(struct cactus *cact, struct cact_env *e,
             struct cact_symbol *key, struct cact_val val)
{
    assert(e);
    assert(key);

    struct cact_env *cur = e;

    do {
        if (TABLE_HAS(cact_env_entries, &cur->entries, key)) {
            TABLE_ENTER(cact_env_entries, &cur->entries, key, val);
            return CACT_UNDEF_VAL;
        }
        cur = e->parent;
    } while (cur != NULL);

    return cact_make_error(cact, "cannot set nonexistent variable", CACT_SYM_VAL(key));
}

/*
 * Does the given environment have the given symbol bound?
 */
bool
cact_env_is_bound(struct cact_env *e, struct cact_symbol *key)
{
    assert(e);
    assert(key);

    struct cact_env *cur = e;

    do {
        if (TABLE_HAS(cact_env_entries, &cur->entries, key)) {
            return true;
        }
        cur = e->parent;
    } while (cur != NULL);

    return false;
}

int
cact_env_num_bindings(struct cact_env *e) 
{
	int count = e->entries.count;

	if (e->parent) {
		return count + cact_env_num_bindings(e->parent);
	}

	return count;
}

/*
 * Display an environment to standard output.
 */
void
print_env(struct cact_env *e)
{
    if (!e) {
        return;
    }

    struct TABLE_ENTRY(cact_env_entries) *bucket;

    TABLE_FOREACH_BUCKET(, bucket, &e->entries) {
        if (bucket->state == TABLE_ENTRY_FILLED) {
            printf("%s : ", bucket->key->sym);
            cact_display(bucket->val);
            printf("\n");
        }
    }

    if (e->parent) {
        printf("parent:\n");
        print_env(e->parent);
    }
}

