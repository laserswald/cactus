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

/* The 'free store' of heap-allocated objects. */
struct cact_store {
	struct cact_arena_set *arena_sets;
	size_t sets_len;
};

void 
cact_store_init(struct cact_store *);

void 
cact_store_finish(struct cact_store *);

struct cact_obj *
cact_store_allocate(struct cact_store*, enum cact_obj_type);

bool 
cact_store_needs_sweep(struct cact_store *);

size_t 
cact_store_count(struct cact_store *);

int 
cact_store_sweep(struct cact_store *);

void 
cact_store_show(struct cact_store *);

#endif // CACT_STORE_H

