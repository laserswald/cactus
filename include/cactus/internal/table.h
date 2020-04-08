/*
 * Copyright (c) 2019 Ben Davenport-Ray <ben.davenportray@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
 * This file defines a hash table and a hash set.
 *
 * A hash table is an implementation of an associative array that uses
 * a hashing function on the given key to index into a normal array with
 * integer indices. This makes lookup for hash tables O(1) in the best case.
 */

/* A relatively good hashing function. */
static inline unsigned int
fnv1a_hash(const char* key, const size_t length)
{
	unsigned int hash = 2166136261u;

	for (size_t i = 0; i < length; i++) {
		hash ^= key[i];
		hash *= 16777619;
	}

	return hash;
}


typedef unsigned int (*table_hash_fn)(const void *);
typedef int (*table_cmp_fn)(const void *, const void *);

#define TABLE_INITIAL_SIZE 8

#define TABLE_STATUS_OK    0
#define TABLE_STATUS_NOT_FOUND -1
#define TABLE_STATUS_ERROR -2

#define TABLE_ENTRY_EMPTY 0
#define TABLE_ENTRY_DELETED 1
#define TABLE_ENTRY_FILLED 2

#define TABLE_FIND_AVAILABLE 0
#define TABLE_FIND_VALUE 1

#define TABLE_ENTRY(name) name##_table_entry

#define TABLE_DECL(name, keytype, valtype) \
struct TABLE_ENTRY(name) { \
	keytype key; \
	valtype val; \
	int state; \
};\
struct name { \
	struct TABLE_ENTRY(name) *entries; \
	unsigned int count; \
	size_t available; \
	table_hash_fn hashfn; \
	table_cmp_fn cmpfn; \
};

#define STRING_TABLE_DECL(name, valtype) \
	TABLE_DECL(name, char*, valtype)

#define STRING_TABLE_INIT(name)	  \
	TABLE_INIT(name, table_string_hash, table_string_cmp)

#define TABLE_SPACE(tab)                 ((tab)->available)
#define TABLE_FIND(name, tab, key)	 name##_TABLE_FIND((tab), (key))
#define TABLE_HAS(name, tab, key)	     (name##_TABLE_FIND((tab), (key)) != NULL)
#define TABLE_ENTER(name, tab, key, val) name##_TABLE_ENTER(tab, key, val)
#define TABLE_REMOVE(name, tab, key)	 name##_TABLE_REMOVE(tab, key)

#define TABLE_INIT(t, hash, cmp) do { \
	(t)->count = 0; \
	(t)->entries = NULL; \
	(t)->available = 0; \
	(t)->hashfn = hash; \
	(t)->cmpfn = cmp; \
} while (0)

#define TABLE_CLEAR(tab) do { \
    if ((tab)->entries != NULL) { \
		xfree((tab)->entries); \
    } \
    (tab)->entries = NULL; \
	(tab)->count = 0; \
	(tab)->available = 0; \
} while (0)

#define TABLE_FOREACH_BUCKET(n, e, h) \
	int __table_foreach_idx; \
	for (__table_foreach_idx = 0, e = &(h)->entries[__table_foreach_idx]; \
		 __table_foreach_idx < TABLE_SPACE(h); \
		 __table_foreach_idx++, e = &(h)->entries[__table_foreach_idx])

/* Generate functions for the hash table. */
#define TABLE_GENERATE(name, keytype, valtype) \
\
/* Find the first filled entry with the key. */ \
static int \
name##_TABLE_FIND_INDEX(struct name *table, keytype key, int what) \
{ \
	if (! table) { \
		return TABLE_STATUS_ERROR; \
	} \
    if (0 == TABLE_SPACE(table)) { \
        return TABLE_STATUS_NOT_FOUND; \
    } \
	uint32_t bucket_idx = (table)->hashfn(key) % TABLE_SPACE(table); \
	uint32_t current = bucket_idx; \
	int first_sentinel = -1; \
	do { \
		assert(current >= 0); \
		assert(current < TABLE_SPACE(table)); \
		struct TABLE_ENTRY(name) found = table->entries[current]; \
		switch (found.state) {\
		case TABLE_ENTRY_EMPTY: \
		    return (first_sentinel != -1) ? first_sentinel : current; \
		case TABLE_ENTRY_DELETED: \
		    if (first_sentinel == -1) first_sentinel = current; \
		    break; \
		case TABLE_ENTRY_FILLED: \
		    if (what == TABLE_FIND_VALUE && table->cmpfn(&found.key, &key) == 0) return current; \
		    break; \
		} \
		current = (current + 1) % TABLE_SPACE(table); \
	} while (current != bucket_idx); \
	return TABLE_STATUS_NOT_FOUND; \
} \
\
/* Ensure the hasstruct nle has the given size. */ \
static int \
name##_TABLE_GROW(struct name *tab, size_t newsize) \
{ \
	if (! tab) { \
		return TABLE_STATUS_ERROR; \
	} \
	if (newsize < (tab)->available) { \
		return TABLE_STATUS_OK; \
	} \
	struct TABLE_ENTRY(name) *old_entries = tab->entries; \
	size_t old_entries_len = tab->available; \
	tab->entries = calloc(newsize, sizeof(struct TABLE_ENTRY(name))); \
	tab->available = newsize; \
	struct TABLE_ENTRY(name) p; \
	for (size_t i = 0; i < old_entries_len; i++) { \
		p = old_entries[i]; \
		if (p.state != TABLE_ENTRY_FILLED) { \
			continue; \
		} \
		int index = name##_TABLE_FIND_INDEX(tab, p.key, TABLE_FIND_AVAILABLE); \
		tab->entries[index].key = p.key; \
		tab->entries[index].val = p.val; \
		tab->entries[index].state = TABLE_ENTRY_FILLED; \
	} \
	free(old_entries); \
	return TABLE_STATUS_OK; \
} \
 \
/* Find a value by a key, or returns NULL. */ \
valtype* \
name##_TABLE_FIND(struct name *h, keytype key) \
{ \
	if (! h) { \
		return NULL; \
	} \
	int bucketidx = name##_TABLE_FIND_INDEX(h, key, TABLE_FIND_VALUE); \
	if (bucketidx < 0) { \
		return NULL; \
	} \
	struct TABLE_ENTRY(name) *found = &(h->entries[bucketidx]); \
	if (! found->key) { \
		return NULL; \
	} \
	return &(found->val); \
} \
 \
int \
name##_TABLE_ENTER(struct name *h, keytype key, valtype item) \
{ \
	if (! h) { \
		return TABLE_STATUS_ERROR; \
	} \
	int index = name##_TABLE_FIND_INDEX(h, key, TABLE_FIND_VALUE); \
	if (index == TABLE_STATUS_NOT_FOUND) { \
		if (h->available < TABLE_INITIAL_SIZE) { \
			name##_TABLE_GROW(h, TABLE_INITIAL_SIZE); \
		} else { \
			name##_TABLE_GROW(h, h->available * 2); \
		} \
		index = name##_TABLE_FIND_INDEX(h, key, TABLE_FIND_VALUE); \
	} \
	assert(index >= 0); \
	h->entries[index].key = key; \
	h->entries[index].val = item; \
	h->entries[index].state = TABLE_ENTRY_FILLED; \
	h->count++; \
	return TABLE_STATUS_OK; \
} \
\
int \
name##_TABLE_REMOVE(struct name *h, keytype key) \
{ \
	if (! h) { \
		return TABLE_STATUS_ERROR; \
	} \
	int index = name##_TABLE_FIND_INDEX(h, key, TABLE_FIND_VALUE); \
	if (index < 0) { \
		return TABLE_STATUS_OK; \
	} \
	struct TABLE_ENTRY(name) found = h->entries[index]; \
	if (found.state == TABLE_ENTRY_FILLED) { \
		found.state = TABLE_ENTRY_DELETED; \
	} \
	return TABLE_STATUS_OK; \
}

#define STRING_TABLE_GENERATE(name, valtype) \
static unsigned int \
table_string_hash(void *key) \
{ \
	return fnv1a_hash(key, strlen(key)); \
} \
static unsigned int \
table_string_cmp(void *a, void *b) \
{ \
	return strcmp((const char*) a, (const char*) b); \
} \
TABLE_GENERATE(name, const char*, valtype)

#endif // TABLE_H

