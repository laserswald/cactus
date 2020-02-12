#include "store.h"

/*
 * Arena operations.
 */

bool cact_arena_has(struct cact_arena *arena, void *thing)
{
    void *first_item = arena->data;
    void *last_item = arena->data + (arena->element_sz * 64);

    return thing >= first_item && thing < last_item;
}

bool cact_arena_is_full(struct cact_arena *arena)
{
    return 0 == ~arena->occupied_set;
}

size_t cact_arena_next_open(struct cact_arena *arena)
{
    size_t i;
    for (i = 0; i < 64; i++) {
        if (0 == arena->occupied_set & (1 << i)) {
            return i;
        }
    }
    return 64;
}

void *cact_arena_get_next(struct cact_arena *arena)
{
    size_t open_slot = cact_arena_next_open(arena);

    arena->occupied_set |= (1 << open_slot);

    return (void*) (((char*)arena) + (open_slot * arena->element_sz));
}

/*
 * Arena set operations.
 */

void cact_arena_set_init(struct cact_arena_set *set, size_t elt_sz)
{
    ARRAY_INIT(set);

    struct cact_arena initial_arena;
    initial_arena.element_sz = elt_sz;
    initial_arena.data = xcalloc(64, initial_arena.element_sz);
    initial_arena.occupied_set = 0;

    ARRAY_ADD(set, initial_arena);
}

void *cact_arena_set_allocate(struct cact_arena_set *set)
{
    int i;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        if (! cact_arena_is_full(ARRAY_ITEM(set, i))) {
            break;
        }
    }

    if (i == ARRAY_LENGTH(set)) {
        // Allocate new arena
    }

    struct cact_arena *arena = &ARRAY_ITEM(set, i);

    return cact_arena_get_next(arena);
}

/*
 * Store operations.
 */

void cact_store_init(struct cact_store *store)
{
    cact_arena_set_init(&store->arenas[CACT_OBJ_PAIR], sizeof(struct cact_pair));
    cact_arena_set_init(&store->arenas[CACT_OBJ_PROCEDURE], sizeof(struct cact_proc));
    cact_arena_set_init(&store->arenas[CACT_OBJ_ENV], sizeof(struct cact_env));
}

struct cact_obj cact_store_allocate(struct cact_store *store, enum cact_obj_type type)
{
    struct cact_obj obj;

    obj.type = type;
    obj.ptr = cact_arena_set_allocate(store->arena[type]);

    return obj;
}

