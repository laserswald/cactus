
#include "cactus/store.h"

#include "cactus/pair.h"
#include "cactus/str.h"
#include "cactus/sym.h"
#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/err.h"

#include "cactus/internal/xmalloc.h"
#include "cactus/internal/utils.h"

/*
 * Arena operations.
 */

void
cact_arena_init(struct cact_arena *arena, size_t element_sz)
{
    arena->element_sz = element_sz;
    arena->data = xcalloc(64, element_sz);
    arena->occupied_set = 0;
}

/* Does this arena have this pointer? */
bool
cact_arena_has(struct cact_arena *arena, void *thing)
{
    void *first_item = arena->data;
    void *last_item = (char*) arena->data + (arena->element_sz * 64);

    return thing >= first_item && thing < last_item;
}

bool
cact_arena_is_full(struct cact_arena *arena)
{
    return 0 == ~arena->occupied_set;
}

size_t
cact_arena_next_open(struct cact_arena *arena)
{
    size_t	i;

    for (i = 0; i < 64; i++)
    {
        if (0 == (arena->occupied_set & (1 << i)))
        {
            break;
        }
    }

    return i;
}

void *
cact_arena_get_next(struct cact_arena *arena)
{
    size_t open_slot = cact_arena_next_open(arena);
    arena->occupied_set |= (1 << open_slot);
    return (void*) (((char*)arena->data) + (open_slot * arena->element_sz));
}

void
cact_arena_finish(struct cact_arena *arena)
{
    xfree(arena->data);
}

/*
 * Arena set operations.
 */

void
cact_arena_set_init(struct cact_arena_set *set, size_t elt_sz)
{
    ARRAY_INIT(set);
    struct cact_arena initial_arena;
    cact_arena_init(&initial_arena, elt_sz);
    ARRAY_ADD(set, initial_arena);
}

struct cact_obj *
cact_arena_set_allocate(struct cact_arena_set *set)
{
    int i;
    for (i = 0; i < ARRAY_LENGTH(set); i++)
    {
        if (! cact_arena_is_full(&ARRAY_ITEM(set, i)))
        {
            break;
        }
    }

    if (i == ARRAY_LENGTH(set))
    {
        // Allocate new arena
        struct cact_arena new_arena;
        new_arena.element_sz = (ARRAY_ITEM(set, 1)).element_sz;
        new_arena.data = xcalloc(64, new_arena.element_sz);
        new_arena.occupied_set = 0;

        ARRAY_ADD(set, new_arena);
    }

    struct cact_arena *arena = &ARRAY_ITEM(set, i);

    size_t slot = cact_arena_next_open(arena);
    struct cact_obj *object = cact_arena_get_next(arena);
    object->store_data.arena = arena;
    object->store_data.place = slot;

    return object;
}

int
cact_arena_set_clean(struct cact_arena_set *set)
{
    int i, count = 0;
    for (i = 0; i < ARRAY_LENGTH(set); i++)
    {
        if (cact_arena_is_full(&ARRAY_ITEM(set, i)))
        {
            ARRAY_REMOVE(set, i);
            count++;
        }
    }
    return count;
}

void
cact_arena_set_finish(struct cact_arena_set *set)
{
    int i;
    for (i = 0; i < ARRAY_LENGTH(set); i++)
    {
        cact_arena_finish(&ARRAY_ITEM(set, i));
    }
    ARRAY_FREE(set);
}

/*
 * Store operations.
 */

void
cact_store_init(struct cact_store *store)
{
    struct
    {
        enum cact_obj_type type;
        size_t size;
    } set_defs[] =
    {
        {CACT_OBJ_PAIR, sizeof(struct cact_pair)},
        {CACT_OBJ_STRING, sizeof(struct cact_string)},
        {CACT_OBJ_PROCEDURE, sizeof(struct cact_proc)},
        {CACT_OBJ_ENVIRONMENT, sizeof(struct cact_env)},
        {CACT_OBJ_ERROR, sizeof(struct cact_error)},
    };

    store->sets_len = LENGTH(set_defs);
    store->arena_sets = xcalloc(store->sets_len, sizeof(struct cact_arena_set));

    int i;
    for (i = 0; i < store->sets_len; i++)
    {
        cact_arena_set_init(&store->arena_sets[set_defs[i].type], set_defs[i].size);
    }
}

struct cact_obj *
cact_store_allocate(struct cact_store *store, enum cact_obj_type type)
{
    struct cact_obj *object;
    object = cact_arena_set_allocate(&store->arena_sets[type]);
    object->type = type;
    return object;
}

void
cact_store_finish(struct cact_store *store)
{
    size_t i;
    for (i = 0; i < store->sets_len; i++)
    {
        cact_arena_set_finish(&store->arena_sets[i]);
    }
    xfree(store->arena_sets);
    store->sets_len = 0;
}

void
cact_store_destroy(struct cact_store *store, struct cact_obj *obj)
{
}

