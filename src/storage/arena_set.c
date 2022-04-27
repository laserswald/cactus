#include <assert.h>
#include <string.h>

#include "arena_set.h"
#include "obj.h"

void
cact_arena_set_init(cact_arena_set_t *set, size_t elt_sz)
{
    assert(set);
    assert(elt_sz > 0);

    ARRAY_INIT(set);

    cact_arena_t initial_arena = {0};
    cact_arena_init(&initial_arena, elt_sz);
    ARRAY_ADD(set, initial_arena);
}

void
cact_arena_set_finish(cact_arena_set_t *set)
{
    assert(set);

    int i;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        cact_arena_finish(&ARRAY_ITEM(set, i));
    }

    ARRAY_FREE(set);
}

int
cact_arena_set_count(cact_arena_set_t *set)
{
    assert(set);

    int i, count;

    count = 0;

    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        count += cact_arena_count(&ARRAY_ITEM(set, i));
    }

    return count;
}

cact_object_t *
cact_arena_set_allocate(cact_arena_set_t *set)
{
    assert(set);

    int i;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        if (! cact_arena_is_full(&ARRAY_ITEM(set, i))) {
            break;
        }
    }

    if (i == ARRAY_LENGTH(set)) {
        // Allocate new arena
        cact_arena_t new_arena = {0};
        cact_arena_init(&new_arena, (ARRAY_ITEM(set, 0)).element_sz);
        ARRAY_ADD(set, new_arena);
    }

    cact_arena_t *arena = &ARRAY_ITEM(set, i);

    size_t slot = cact_arena_next_open(arena);
    cact_object_t *object = cact_arena_get_next(arena);
    object->store_data.arena = arena;
    object->store_data.place = slot;
    object->store_data.mark = CACT_STORE_MARK_OCCUPIED;

    return object;
}

size_t
cact_arena_set_sweep(cact_arena_set_t *set)
{
    assert(set);

    int i, count = 0;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        count += cact_arena_sweep(&ARRAY_ITEM(set, i));
    }
    return count;
}

int
cact_arena_set_clean(cact_arena_set_t *set)
{
    assert(set);

    int i, count = 0;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        if (cact_arena_is_full(&ARRAY_ITEM(set, i))) {
            ARRAY_REMOVE(set, i);
            count++;
        }
    }
    return count;
}
