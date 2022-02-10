#ifndef __CACT_ARENA_H__
#define __CACT_ARENA_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint64_t cact_bitset_t;

/*
 * This is the main owner of the memory allocated by the 
 * garbage collector. 
 * 
 * It consists of a 64-item wide array with a bitset of all the 
 * occupied slots.
 */

struct cact_arena {
	void *data;
	size_t element_sz;
	cact_bitset_t occupied_set; /* A bitset */
};

/* Initialize an arena to hold items of a certain size. */
void
cact_arena_init(struct cact_arena *arena, const size_t element_sz);

/* Release the storage for this arena. */
void
cact_arena_finish(struct cact_arena *arena);

/* Is this arena full of items? */
bool
cact_arena_is_full(const struct cact_arena *arena);

/* Get the object at the given slot in this arena. */
struct cact_obj *
cact_arena_get(struct cact_arena * const arena, const size_t nth);

/* Return true if the pointer is owned by the arena. */
bool 
cact_arena_has(struct cact_arena*, void*);

/* Get the item at the nth spot. */
struct cact_obj *
cact_arena_get(struct cact_arena * const arena, const size_t nth);

/* Return true if the slot is occupied. */
bool
cact_arena_slot_occupied(struct cact_arena *arena, size_t nth);

/* Mark the given slot as open. */
void
cact_arena_mark_open(struct cact_arena *arena, size_t nth);

/* Get the next open slot in the arena. */
size_t
cact_arena_next_open(struct cact_arena *arena);

/* Return the count of all occupied slots in the arena. */
int
cact_arena_count(struct cact_arena *arena);

/* Mark all unreachable objects in the arena as free for use. */
size_t
cact_arena_sweep(struct cact_arena *arena);

/* Get the next available free object in the arena. */
struct cact_obj *
cact_arena_get_next(struct cact_arena *arena);

#endif /* __CACT_ARENA_H__ */
