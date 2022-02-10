#include <assert.h>
#include <string.h>

#include "arena_set.h"
#include "obj.h"

void
cact_arena_set_init(struct cact_arena_set *set, size_t elt_sz)
{
    assert(set);
    assert(elt_sz > 0);

    ARRAY_INIT(set);

    struct cact_arena initial_arena = {0};
    cact_arena_init(&initial_arena, elt_sz);
    ARRAY_ADD(set, initial_arena);
}

void
cact_arena_set_finish(struct cact_arena_set *set)
{
    assert(set);

    int i;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        cact_arena_finish(&ARRAY_ITEM(set, i));
    }

    ARRAY_FREE(set);
}

int
cact_arena_set_count(struct cact_arena_set *set)
{
    assert(set);

    int i, count;

    count = 0;

    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        count += cact_arena_count(&ARRAY_ITEM(set, i));
    }

    return count;
}

struct cact_obj *
cact_arena_set_allocate(struct cact_arena_set *set)
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
        struct cact_arena new_arena = {0};
        cact_arena_init(&new_arena, (ARRAY_ITEM(set, 0)).element_sz);
        ARRAY_ADD(set, new_arena);
    }

    struct cact_arena *arena = &ARRAY_ITEM(set, i);

    size_t slot = cact_arena_next_open(arena);
    struct cact_obj *object = cact_arena_get_next(arena);
    object->store_data.arena = arena;
    object->store_data.place = slot;
    object->store_data.mark = CACT_STORE_MARK_OCCUPIED;

    return object;
}

size_t
cact_arena_set_sweep(struct cact_arena_set *set)
{
    assert(set);

    int i, count = 0;
    for (i = 0; i < ARRAY_LENGTH(set); i++) {
        count += cact_arena_sweep(&ARRAY_ITEM(set, i));
    }
    return count;
}

int
cact_arena_set_clean(struct cact_arena_set *set)
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
