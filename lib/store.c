
#include "cactus/store.h"

#include "cactus/core.h"
#include "cactus/pair.h"
#include "cactus/str.h"
#include "cactus/sym.h"
#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/err.h"

#include "cactus/internal/xmalloc.h"
#include "cactus/internal/utils.h"

#define CACT_GC_STRESS

/*
 * Arena operations.
 */

void
cact_arena_init(struct cact_arena *arena, size_t element_sz)
{
	arena->element_sz = element_sz;
	arena->data = xcalloc(64, element_sz);
	arena->occupied_set = 0;
}

bool
cact_arena_is_full(struct cact_arena *arena)
{
	return 0 == ~arena->occupied_set;
}

struct cact_obj *
cact_arena_get(struct cact_arena *arena, size_t nth)
{
	return (struct cact_obj *) (((char*)arena->data) + (nth * arena->element_sz));
}

bool
cact_arena_slot_occupied(struct cact_arena *arena, size_t nth)
{
	return 0 != (arena->occupied_set & (1ll << nth));
}

void
cact_arena_mark_open(struct cact_arena *arena, size_t nth)
{
	arena->occupied_set &= ~(1ll << nth);
}

/* Does this arena have this pointer? */
bool
cact_arena_has(struct cact_arena *arena, void *thing)
{
	void *first_item = arena->data;
	void *last_item = (char*) arena->data + (arena->element_sz * 64);

	return thing >= first_item && thing < last_item;
}

size_t
cact_arena_next_open(struct cact_arena *arena)
{
	size_t	i;

	for (i = 0; i < 64; i++) {
		if (! cact_arena_slot_occupied(arena, i)) {
			break;
		}
	}

	return i;
}

int
cact_arena_count(struct cact_arena *arena)
{
	size_t	i;
	int     count;

	count = 0;

	for (i = 0; i < 64; i++) {
		if (cact_arena_slot_occupied(arena, i)) {
			count++;
		}
	}

	return count;
}

size_t
cact_arena_sweep(struct cact_arena *arena)
{
	struct cact_obj *obj;
	size_t	i, swept;

    obj = NULL;
	swept = 0;

	for (i = 0; i < 64; i++) {
		if (cact_arena_slot_occupied(arena, i)) {
			obj = cact_arena_get(arena, i);
			if (obj->store_data.mark != CACT_STORE_MARK_REACHABLE) {
				printf("; cactus gc: unreachable object: ");
				fprint_obj(stdout, obj);
				printf("\n");
				obj->store_data.mark = CACT_STORE_MARK_FREE;
				cact_obj_destroy(obj);
				cact_arena_mark_open(arena, i);
				swept++;
			} else {
				obj->store_data.mark = CACT_STORE_MARK_OCCUPIED;
			}
		}
	}

	return swept;
}

struct cact_obj *
cact_arena_get_next(struct cact_arena *arena)
{
	size_t open_slot = cact_arena_next_open(arena);
    size_t count = cact_arena_count(arena);
	arena->occupied_set |= (1ll << open_slot);
	assert(cact_arena_count(arena) == 1 + count);
	return cact_arena_get(arena, open_slot);
}

void
cact_arena_finish(struct cact_arena *arena)
{
	xfree(arena->data);
}

/*
 * Arena set operations.
 */

void
cact_arena_set_init(struct cact_arena_set *set, size_t elt_sz)
{
	ARRAY_INIT(set);
	struct cact_arena initial_arena = {0}; 
	cact_arena_init(&initial_arena, elt_sz);
	ARRAY_ADD(set, initial_arena);
}

int
cact_arena_set_count(struct cact_arena_set *set)
{
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
	int i, count = 0;
	for (i = 0; i < ARRAY_LENGTH(set); i++) {
		count += cact_arena_sweep(&ARRAY_ITEM(set, i));
	}
	return count;
}

int
cact_arena_set_clean(struct cact_arena_set *set)
{
	int i, count = 0;
	for (i = 0; i < ARRAY_LENGTH(set); i++) {
		if (cact_arena_is_full(&ARRAY_ITEM(set, i))) {
			ARRAY_REMOVE(set, i);
			count++;
		}
	}
	return count;
}

void
cact_arena_set_finish(struct cact_arena_set *set)
{
	int i;
	for (i = 0; i < ARRAY_LENGTH(set); i++) {
		cact_arena_finish(&ARRAY_ITEM(set, i));
	}
	ARRAY_FREE(set);
}

/*
 * Store operations.
 */

void
cact_store_init(struct cact_store *store)
{
	struct {
		enum cact_obj_type type;
		size_t size;
	} set_defs[] = {
		{CACT_OBJ_PAIR, sizeof(struct cact_pair)},
		{CACT_OBJ_STRING, sizeof(struct cact_string)},
		{CACT_OBJ_PROCEDURE, sizeof(struct cact_proc)},
		{CACT_OBJ_ENVIRONMENT, sizeof(struct cact_env)},
		{CACT_OBJ_CONT, sizeof(struct cact_cont)},
		{CACT_OBJ_ERROR, sizeof(struct cact_error)},
	};

	store->sets_len = LENGTH(set_defs);
	store->arena_sets = xcalloc(store->sets_len, sizeof(struct cact_arena_set));

	int i;
	for (i = 0; i < store->sets_len; i++) {
		cact_arena_set_init(&store->arena_sets[set_defs[i].type], set_defs[i].size);
	}
}

size_t
cact_store_count(struct cact_store *store)
{
	size_t occupied;

    occupied = 0;

	int i;
	for (i = 0; i < store->sets_len; i++) {
		occupied += cact_arena_set_count(&store->arena_sets[i]);
	}

	return occupied;
}

void
cact_store_show(struct cact_store *store)
{
	const char *objnames[] = {
		"pair", "string", "procedure", "env", "cont", "error",
	};

	printf("storage totals:\n");

	int i;
	for (i = 0; i < store->sets_len; i++) {
		printf("\t%s: %d\n", objnames[i], cact_arena_set_count(&store->arena_sets[i]));
	}
}

bool
cact_store_needs_sweep(struct cact_store *store)
{
#ifdef CACT_GC_STRESS
    return true;
#else
	size_t occupied, capacity;

    occupied = 0;
    capacity = 0;

	int i;
	for (i = 0; i < store->sets_len; i++) {
		occupied += cact_arena_set_count(&store->arena_sets[i]);
		capacity += ARRAY_LENGTH(&store->arena_sets[i]) * 64;
	}

	return (occupied / capacity) >= 0.8;
#endif /* CACT_GC_STRESS */
}

int
cact_store_sweep(struct cact_store *store)
{
	int i, swept = 0;

	for (i = 0; i < store->sets_len; i++) {
		swept += cact_arena_set_sweep(&store->arena_sets[i]);
	}

	return swept;
}

struct cact_obj *
cact_store_allocate(struct cact_store *store, enum cact_obj_type type)
{
	struct cact_obj *object;
	object = cact_arena_set_allocate(&store->arena_sets[type]);
	object->type = type;
	return object;
}

void
cact_store_finish(struct cact_store *store)
{
	size_t i;
	for (i = 0; i < store->sets_len; i++) {
		cact_arena_set_finish(&store->arena_sets[i]);
	}
	xfree(store->arena_sets);
	store->sets_len = 0;
}

void
cact_store_destroy(struct cact_store *store, struct cact_obj *obj)
{
}

