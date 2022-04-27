#ifndef __CACT_ARENA_H__
#define __CACT_ARENA_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "storage/obj.h"

typedef uint64_t cact_bitset_t;

/*
 * This is the main owner of the memory allocated by the 
 * garbage collector. 
 * 
 * It consists of a 64-item wide array with a bitset of all the 
 * occupied slots.
 */

typedef struct cact_arena {
	void *data;
	size_t element_sz;
	cact_bitset_t occupied_set; /* A bitset */
} cact_arena_t;

/* Initialize an arena to hold items of a certain size. */
void
cact_arena_init(cact_arena_t *arena, const size_t element_sz);

/* Release the storage for this arena. */
void
cact_arena_finish(cact_arena_t *arena);

/* Is this arena full of items? */
bool
cact_arena_is_full(const cact_arena_t *arena);

/* Get the object at the given slot in this arena. */
cact_object_t *
cact_arena_get(cact_arena_t * const arena, const size_t nth);

/* Return true if the pointer is owned by the arena. */
bool 
cact_arena_has(cact_arena_t*, void*);

/* Get the item at the nth spot. */
cact_object_t *
cact_arena_get(cact_arena_t * const arena, const size_t nth);

/* Return true if the slot is occupied. */
bool
cact_arena_slot_occupied(cact_arena_t *arena, size_t nth);

/* Mark the given slot as open. */
void
cact_arena_mark_open(cact_arena_t *arena, size_t nth);

/* Get the next open slot in the arena. */
size_t
cact_arena_next_open(cact_arena_t *arena);

/* Return the count of all occupied slots in the arena. */
int
cact_arena_count(cact_arena_t *arena);

/* Mark all unreachable objects in the arena as free for use. */
size_t
cact_arena_sweep(cact_arena_t *arena);

/* Get the next available free object in the arena. */
cact_object_t *
cact_arena_get_next(cact_arena_t *arena);

#endif /* __CACT_ARENA_H__ */
