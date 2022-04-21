#include "store.h"

#include "core.h"
#include "pair.h"
#include "str.h"
#include "sym.h"
#include "proc.h"
#include "env.h"
#include "err.h"
#include "vec.h"
#include "port.h"

#include "storage/arena_set.h"

#include "internal/xmalloc.h"
#include "internal/utils.h"

#define CACT_GC_STRESS

/*
 * Store operations.
 */

void
cact_store_init(struct cact_store *store)
{
    assert(store);

    struct {
        enum cact_obj_type type;
        size_t size;
    } set_defs[] = {
        {CACT_OBJ_PAIR, sizeof(struct cact_pair)},
        {CACT_OBJ_STRING, sizeof(struct cact_string)},
        {CACT_OBJ_PROCEDURE, sizeof(struct cact_proc)},
        {CACT_OBJ_ENVIRONMENT, sizeof(struct cact_env)},
        {CACT_OBJ_CONT, sizeof(struct cact_cont)},
        {CACT_OBJ_ERROR, sizeof(struct cact_error)},
        {CACT_OBJ_PORT, sizeof(struct cact_port)},
        {CACT_OBJ_VECTOR, sizeof(struct cact_vec)},
    };

    store->sets_len = LENGTH(set_defs);
    store->arena_sets = xcalloc(store->sets_len, sizeof(struct cact_arena_set));

    int i;
    for (i = 0; i < store->sets_len; i++) {
        cact_arena_set_init(&store->arena_sets[set_defs[i].type], set_defs[i].size);
    }
}

void
cact_store_finish(struct cact_store *store)
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
cact_store_count(struct cact_store *store)
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
cact_store_show(struct cact_store *store)
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

#undef CACT_GC_STRESS
bool
cact_store_needs_sweep(struct cact_store *store)
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
cact_store_sweep(struct cact_store *store)
{
    assert(store);

    int i, swept = 0;

    for (i = 0; i < store->sets_len; i++) {
        swept += cact_arena_set_sweep(&store->arena_sets[i]);
    }

    return swept;
}

struct cact_obj *
cact_store_allocate(struct cact_store *store, enum cact_obj_type type)
{
    assert(store);

    struct cact_obj *object;

    object = cact_arena_set_allocate(&store->arena_sets[type]);
    object->type = type;

    return object;
}

