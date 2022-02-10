
#include <assert.h>
#include "arena.h"
#include "obj.h"
#include "internal/xmalloc.h"

/*
 * Arena operations.
 */

void
cact_arena_init(struct cact_arena *arena, const size_t element_sz)
{
    assert(arena);
    assert(element_sz > 0);

    arena->element_sz = element_sz;
    arena->data = xcalloc(64, element_sz);
    arena->occupied_set = 0;
}

void
cact_arena_finish(struct cact_arena *arena)
{
    if (! arena) {
        return;
    }

    xfree(arena->data);
}

bool
cact_arena_is_full(const struct cact_arena *arena)
{
    assert(arena);

    return 0 == ~arena->occupied_set;
}

struct cact_obj *
cact_arena_get(struct cact_arena * const arena, const size_t nth)
{
    assert(arena);
    assert(nth < 64);

    return (struct cact_obj *) (((char*)arena->data) + (nth * arena->element_sz));
}

bool
cact_arena_slot_occupied(struct cact_arena *arena, size_t nth)
{
    assert(arena);
    assert(nth < 64);

    return 0 != (arena->occupied_set & (1ll << nth));
}

void
cact_arena_mark_open(struct cact_arena *arena, size_t nth)
{
    assert(arena);
    assert(nth < 64);

    arena->occupied_set &= ~(1ll << nth);
}

bool
cact_arena_has(struct cact_arena *arena, void *thing)
{
    assert(arena);
    assert(thing);

    void *first_item = arena->data;
    void *last_item = (char*) arena->data + (arena->element_sz * 64);

    return thing >= first_item && thing < last_item;
}

size_t
cact_arena_next_open(struct cact_arena *arena)
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
cact_arena_count(struct cact_arena *arena)
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
cact_arena_sweep(struct cact_arena *arena)
{
    assert(arena);

    struct cact_obj *obj;
    size_t i, swept;

    obj = NULL;
    swept = 0;

    for (i = 0; i < 64; i++) {
        if (cact_arena_slot_occupied(arena, i)) {
            obj = cact_arena_get(arena, i);
            if (obj->store_data.mark != CACT_STORE_MARK_REACHABLE) {
                obj->store_data.mark = CACT_STORE_MARK_FREE;
                cact_obj_destroy(obj);
                cact_arena_mark_open(arena, i);
                swept++;
            } else {
                obj->store_data.mark = CACT_STORE_MARK_OCCUPIED;
            }
        }
    }

    return swept;
}

struct cact_obj *
cact_arena_get_next(struct cact_arena *arena)
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

