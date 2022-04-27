
#include <assert.h>
#include "arena.h"
#include "obj.h"
#include "internal/xmalloc.h"

/*
 * Arena operations.
 */

void
cact_arena_init(cact_arena_t *arena, const size_t element_sz)
{
    assert(arena);
    assert(element_sz > 0);

    arena->element_sz = element_sz;
    arena->data = xcalloc(64, element_sz);
    arena->occupied_set = 0;
}

void
cact_arena_finish(cact_arena_t *arena)
{
    if (! arena) {
        return;
    }

    xfree(arena->data);
}

bool
cact_arena_is_full(const cact_arena_t *arena)
{
    assert(arena);

    return 0 == ~arena->occupied_set;
}

cact_object_t *
cact_arena_get(cact_arena_t * const arena, const size_t nth)
{
    assert(arena);
    assert(nth < 64);

    return (cact_object_t *) (((char*)arena->data) + (nth * arena->element_sz));
}

bool
cact_arena_slot_occupied(cact_arena_t *arena, size_t nth)
{
    assert(arena);
    assert(nth < 64);

    return 0 != (arena->occupied_set & (1ll << nth));
}

void
cact_arena_mark_open(cact_arena_t *arena, size_t nth)
{
    assert(arena);
    assert(nth < 64);

    arena->occupied_set &= ~(1ll << nth);
}

bool
cact_arena_has(cact_arena_t *arena, void *thing)
{
    assert(arena);
    assert(thing);

    void *first_item = arena->data;
    void *last_item = (char*) arena->data + (arena->element_sz * 64);

    return thing >= first_item && thing < last_item;
}

size_t
cact_arena_next_open(cact_arena_t *arena)
{
    size_t	i;

    assert(arena);

    for (i = 0; i < 64; i++) {
        if (! cact_arena_slot_occupied(arena, i)) {
            break;
        }
    }

    return i;
}

int
cact_arena_count(cact_arena_t *arena)
{
    assert(arena);

    size_t	i;
    int     count;

    count = 0;

    for (i = 0; i < 64; i++) {
        if (cact_arena_slot_occupied(arena, i)) {
            count++;
        }
    }

    return count;
}

size_t
cact_arena_sweep(cact_arena_t *arena)
{
    assert(arena);

    cact_object_t *obj;
    size_t i, swept;

    obj = NULL;
    swept = 0;

    for (i = 0; i < 64; i++) {
        if (cact_arena_slot_occupied(arena, i)) {
            obj = cact_arena_get(arena, i);
            if (obj->store_data.mark != CACT_STORE_MARK_REACHABLE) {
                obj->store_data.mark = CACT_STORE_MARK_FREE;
                cact_destroy_object(obj);
                cact_arena_mark_open(arena, i);
                swept++;
            } else {
                obj->store_data.mark = CACT_STORE_MARK_OCCUPIED;
            }
        }
    }

    return swept;
}

cact_object_t *
cact_arena_get_next(cact_arena_t *arena)
{
    assert(arena);

    size_t open_slot = cact_arena_next_open(arena);
#ifndef NDEBUG
    size_t count = cact_arena_count(arena);
#endif
    arena->occupied_set |= (1ll << open_slot);
    assert(cact_arena_count(arena) == 1 + count);

    return cact_arena_get(arena, open_slot);
}

