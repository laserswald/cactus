#include "store.h"

#include "core.h"
#include "pair.h"
#include "str.h"
#include "sym.h"
#include "proc.h"
#include "env.h"
#include "err.h"
#include "vec.h"

#include "storage/arena_set.h"

#include "internal/xmalloc.h"
#include "internal/utils.h"

#define CACT_GC_STRESS

/*
 * Store operations.
 */

void
cact_store_init(cact_store_t *store)
{
    assert(store);

    struct {
        cact_object_type_t type;
        size_t size;
    } set_defs[] = {
        {CACT_OBJ_PAIR, sizeof(cact_pair_t)},
        {CACT_OBJ_STRING, sizeof(cact_string_t)},
        {CACT_OBJ_PROCEDURE, sizeof(cact_procedure_t)},
        {CACT_OBJ_ENVIRONMENT, sizeof(cact_env_t)},
        {CACT_OBJ_CONT, sizeof(cact_frame_t)},
        {CACT_OBJ_ERROR, sizeof(cact_error_t)},
        {CACT_OBJ_VECTOR, sizeof(cact_vec_t)},
    };

    store->sets_len = LENGTH(set_defs);
    store->arena_sets = xcalloc(store->sets_len, sizeof(cact_arena_set_t));

    int i;
    for (i = 0; i < store->sets_len; i++) {
        cact_arena_set_init(&store->arena_sets[set_defs[i].type], set_defs[i].size);
    }
}

void
cact_store_finish(cact_store_t *store)
{
    assert(store);

    size_t i;
    for (i = 0; i < store->sets_len; i++) {
        cact_arena_set_finish(&store->arena_sets[i]);
    }

    xfree(store->arena_sets);
    store->arena_sets = NULL;

    store->sets_len = 0;
}

size_t
cact_store_count(cact_store_t *store)
{
    assert(store);

    size_t occupied;

    occupied = 0;

    int i;
    for (i = 0; i < store->sets_len; i++) {
        occupied += cact_arena_set_count(&store->arena_sets[i]);
    }

    return occupied;
}

void
cact_store_show(cact_store_t *store)
{
    assert(store);

    const char *objnames[] = {
        "pair", "string", "procedure", "env", "cont", "error", "vector",
    };

    printf("storage totals:\n");

    int i;
    for (i = 0; i < store->sets_len; i++) {
        printf("\t%s: %d\n", objnames[i], cact_arena_set_count(&store->arena_sets[i]));
    }
}

bool
cact_store_needs_sweep(cact_store_t *store)
{
    assert(store);

#ifdef CACT_GC_STRESS
    return true;
#else
    size_t occupied, capacity;

    occupied = 0;
    capacity = 0;

    int i;
    for (i = 0; i < store->sets_len; i++) {
        occupied += cact_arena_set_count(&store->arena_sets[i]);
        capacity += ARRAY_LENGTH(&store->arena_sets[i]) * 64;
    }

    return (occupied / capacity) >= 0.8;
#endif /* CACT_GC_STRESS */
}

int
cact_store_sweep(cact_store_t *store)
{
    assert(store);

    int i, swept = 0;

    for (i = 0; i < store->sets_len; i++) {
        swept += cact_arena_set_sweep(&store->arena_sets[i]);
    }

    return swept;
}

cact_object_t *
cact_store_allocate(cact_store_t *store, cact_object_type_t type)
{
    assert(store);

    cact_object_t *object;

    object = cact_arena_set_allocate(&store->arena_sets[type]);
    object->type = type;

    return object;
}

