#ifndef CACT_STORE_H
#define CACT_STORE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "internal/array.h"
#include "internal/queue.h"

enum cact_store_marking {
	CACT_STORE_MARK_FREE,
	CACT_STORE_MARK_OCCUPIED,
	CACT_STORE_MARK_REACHABLE
};

/* Bookkeeping data for items that live in the store. */
struct cact_store_data {
	struct cact_arena *arena;
	size_t place;
	enum cact_store_marking mark;
};

#include "obj.h"

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

void cact_store_init(struct cact_store *);
void cact_store_finish(struct cact_store *);
struct cact_obj *cact_store_allocate(struct cact_store*, enum cact_obj_type);
bool cact_store_needs_sweep(struct cact_store *);
size_t cact_store_count(struct cact_store *);
int cact_store_sweep(struct cact_store *);
void cact_store_show(struct cact_store *);

#endif // CACT_STORE_H

