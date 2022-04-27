#ifndef CACT_STORE_H
#define CACT_STORE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "internal/array.h"
#include "internal/queue.h"

typedef struct cact_arena_set cact_arena_set_t;

typedef enum cact_store_marking {
	CACT_STORE_MARK_FREE,
	CACT_STORE_MARK_OCCUPIED,
	CACT_STORE_MARK_REACHABLE
} cact_store_marking_t;

/* Bookkeeping data for items that live in the store. */
typedef struct cact_store_data {
	struct cact_arena *arena;
	size_t place;
	cact_store_marking_t mark;
} cact_store_data_t;

#include "obj.h"

/* The 'free store' of heap-allocated objects. */
typedef struct cact_store {
	cact_arena_set_t *arena_sets;
	size_t sets_len;
} cact_store_t;

void 
cact_store_init(cact_store_t *);

void 
cact_store_finish(cact_store_t *);

cact_object_t *
cact_store_allocate(cact_store_t *, cact_object_type_t);

bool 
cact_store_needs_sweep(cact_store_t *);

size_t 
cact_store_count(cact_store_t *);

int 
cact_store_sweep(cact_store_t *);

void 
cact_store_show(cact_store_t *);

#endif // CACT_STORE_H

