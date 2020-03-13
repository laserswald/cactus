#ifndef CACT_STORE_H
#define CACT_STORE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "cactus/internal/array.h"
#include "cactus/internal/queue.h"

/* Bookkeeping data for items that live in the store. */
struct cact_store_data {
    struct cact_arena *arena;
    size_t place;
    int marked;
};

#include "cactus/obj.h"

struct cact_arena {
    void *data;
    size_t element_sz;
    uint64_t occupied_set; /* A bitset */
};

ARRAY_DECL(cact_arena_set, struct cact_arena);

/* The 'free store' of heap-allocated objects. */
struct cact_store {
    struct cact_arena_set *arena_sets;
    size_t sets_len;
};

bool cact_arena_has(struct cact_arena*, void*);

void cact_arena_set_init(struct cact_arena_set *, size_t);
struct cact_obj *cact_arena_set_allocate(struct cact_arena_set *);

void cact_store_init(struct cact_store*);
struct cact_obj *cact_store_allocate(struct cact_store*, enum cact_obj_type);

#endif // CACT_STORE_H

