#ifndef arena_set_h_INCLUDED
#define arena_set_h_INCLUDED

#include "internal/array.h"

typedef struct cact_arena_set cact_arena_set_t;

#include "storage/arena.h"

/*
 * A set of arenas that provide automatic growth and 
 * cleanup of items of a certain size.
 */

ARRAY_DECL(cact_arena_set, struct cact_arena);

/* Initialize a set of arenas. */
void 
cact_arena_set_init(cact_arena_set_t *, size_t);

/* Release a set of arenas. */
void
cact_arena_set_finish(cact_arena_set_t *set);

/* Return the amount of occupied slots in the entire set. */
int
cact_arena_set_count(cact_arena_set_t *set);

/* Allocate an item from the set. */
cact_object_t *
cact_arena_set_allocate(cact_arena_set_t *);

/* Sweep each arena in the set. */
size_t
cact_arena_set_sweep(cact_arena_set_t *set);

/* Remove any empty arenas in the set. */
int
cact_arena_set_clean(cact_arena_set_t *set);

#endif // arena_set_h_INCLUDED

