
#include <stdlib.h>

#include "core.h"
#include "env.h"
#include "val.h"
#include "sym.h"
#include "write.h"
#include "err.h"

#include "storage/store.h"

#include "internal/utils.h"
#include "internal/table.h"
#include "internal/debug.h"

TABLE_GENERATE(cact_env_entries, cact_symbol_t *, cact_value_t)

/*
 * Create and initialize a new environment.
 */
cact_value_t
cact_make_env(cact_context_t *cact, cact_env_t *parent)
{
    cact_env_t *env = (cact_env_t *)cact_store_allocate(&cact->store, CACT_OBJ_ENVIRONMENT);
    cact_env_init(env, parent);
    return CACT_OBJ_VAL((cact_object_t *) env);
}

/*
 * Initialize a given environment to have no entries and set it's parent.
 */
void
cact_env_init(cact_env_t *e, cact_env_t *parent)
{
    assert(e != parent);

    e->parent = parent;
    TABLE_INIT(&e->entries, ptrhash, cact_symbol_cmp);
}

/*
 * Mark the environment as reachable during garbage collection.
 */
void
cact_mark_env(cact_object_t *o)
{
    cact_env_t *e;
    struct TABLE_ENTRY(cact_env_entries) *bucket;

    e = (cact_env_t *) o;

    TABLE_FOREACH_BUCKET(, bucket, &e->entries) {
        if (bucket->state == TABLE_ENTRY_FILLED) {
            cact_mark_value(bucket->val);
        }
    }

    if (e->parent) {
        cact_mark_env((cact_object_t *) e->parent);
    }
}

/*
 * Destroy any data contained within this environment.
 */
void
cact_destroy_env(cact_object_t *o)
{
    cact_env_t *e;

    e = (cact_env_t *) o;

    TABLE_CLEAR(&e->entries);
}

/*
 * Look up the key in the environment and return it, or raise an error.
 */
cact_value_t
cact_env_lookup(cact_context_t *cact, cact_env_t *e, cact_symbol_t *key)
{
    assert(e);
    assert(key);

    cact_value_t *found = TABLE_FIND(cact_env_entries, &e->entries, key);

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
cact_value_t
cact_env_define(cact_context_t *cact, cact_env_t *e,
                cact_symbol_t *key, cact_value_t val)
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
cact_value_t
cact_env_set(cact_context_t *cact, cact_env_t *e,
             cact_symbol_t *key, cact_value_t val)
{
    assert(e);
    assert(key);

    cact_env_t *cur = e;

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
cact_env_is_bound(cact_env_t *e, cact_symbol_t *key)
{
    assert(e);
    assert(key);

    cact_env_t *cur = e;

    do {
        if (TABLE_HAS(cact_env_entries, &cur->entries, key)) {
            return true;
        }
        cur = e->parent;
    } while (cur != NULL);

    return false;
}

int
cact_env_num_bindings(cact_env_t *e)
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
print_env(cact_env_t *e)
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

