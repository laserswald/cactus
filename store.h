#ifndef CACT_STORE_H
#define CACT_STORE_H

#include <stdlib.h>
#include <stdint.h>
#include "array.h"
#include "obj.h"

/* Bookkeeping data for items that live in the store. */
struct cact_store_data {
    struct cact_arena *arena;
    size_t place;
    int marked;
};

struct cact_arena {
    void *data;
    size_t element_sz;
    uint64_t occupied_set; /* A bitset */
};

ARRAY_DECL(cact_arena_set, struct cact_arena);

void cact_arena_set_init(struct cact_arena_set *, size_t);

/* The 'free store' of heap-allocated objects. */
struct cact_store {
    struct cact_arena_set *arenas;
};

void cact_store_init(struct cact_store*);

struct cact_obj *cact_store_allocate(struct cact_store*);

#endif // CACT_STORE_H

