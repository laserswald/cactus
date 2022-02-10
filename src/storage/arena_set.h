#ifndef arena_set_h_INCLUDED
#define arena_set_h_INCLUDED

#include "storage/arena.h"
#include "internal/array.h"

/*
 * A set of arenas that provide automatic growth and 
 * cleanup of items of a certain size.
 */

ARRAY_DECL(cact_arena_set, struct cact_arena);

/* Initialize a set of arenas. */
void 
cact_arena_set_init(struct cact_arena_set *, size_t);

/* Release a set of arenas. */
void
cact_arena_set_finish(struct cact_arena_set *set);

/* Return the amount of occupied slots in the entire set. */
int
cact_arena_set_count(struct cact_arena_set *set);

/* Allocate an item from the set. */
struct cact_obj *
cact_arena_set_allocate(struct cact_arena_set *);

/* Sweep each arena in the set. */
size_t
cact_arena_set_sweep(struct cact_arena_set *set);

/* Remove any empty arenas in the set. */
int
cact_arena_set_clean(struct cact_arena_set *set);

#endif // arena_set_h_INCLUDED

