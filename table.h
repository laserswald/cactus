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

/*
 * This file defines a hash table data structure with string keys.
 * 
 * A hash table is an implementation of an associative array that uses
 * a hashing function on the given key to index into a normal array with
 * integer indices. This makes lookup for hash tables O(1) in the best case.
 */ 

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define TABLE_STATUS_OK 0
#define TABLE_STATUS_ERROR 1
#define TABLE_SENTINEL_KEY (void*)0xDEADBEEF /* Change this if there's a problem */

#define TABLE_INITIALIZER {NULL, 0, 0}

#define TABLE_DECL(n, c) \
struct n##_hash_entry { \
	char	*key; \
	c	*val; \
}; \
struct n { \
	struct n##_hash_entry *entries; \
	unsigned int count; \
	size_t available; \
};

#define TABLE_SPACE(tab) ((tab)->available)

#define TABLE_INITIAL_SIZE 8

/* A relatively good hashing function. */
static inline uint32_t 
fnv1a_hash(const char* key, const size_t length) 
{
	uint32_t hash = 2166136261u;

	for (int i = 0; i < length; i++) {
		hash ^= key[i];
		hash *= 16777619;
	}

	return hash;
} 

#define TABLE_INIT(t) do { \
	(t)->count = 0; \
	(t)->entries = NULL; \
	(t)->available = 0; \
} while (0)

#define TABLE_CLEAR(tab) do { \
	free((tab)->entries); \
	(tab)->count = 0; \
	(tab)->available = 0; \
} while (0)

#define TABLE_FOREACH_BUCKET(n, e, h) \
	int __table_foreach_idx; \
	for (__table_foreach_idx = 0, e = &(h)->entries[__table_foreach_idx]; \
		 __table_foreach_idx < TABLE_SPACE(h); \
		 __table_foreach_idx++, e = &(h)->entries[__table_foreach_idx]) 

/* Generate functions for the hash table. */
#define TABLE_GENERATE(n, c) \
\
/* Find the first filled entry with the key. */ \
static struct n##_hash_entry * \
n##_HASH_FIND_FILLED_ENTRY(struct n##_hash_entry *entries, size_t entrieslen, char *str) \
{ \
	assert(str); \
	if (! entries || entrieslen == 0) { \
		return NULL; \
	} \
	uint32_t bucket_idx = fnv1a_hash(str, strlen(str)) % entrieslen; \
	uint32_t current = bucket_idx; \
	struct n##_hash_entry *found = NULL; \
	do { \
		assert(current >= 0); \
		assert(current < entrieslen); \
		found = &(entries[current]); \
    		if (found->key == NULL) { \
			break; \
		} else if (strcmp(found->key, str) == 0) { \
			return found; \
		} \
		current = (current + 1) % entrieslen; \
	} while (current != bucket_idx); \
	return NULL; \
} \
\
/* Find the bucket corresponding to the key. */ \
static struct n##_hash_entry * \
n##_HASH_FIND_EMPTY_ENTRY(struct n##_hash_entry *entries, size_t entrieslen, char *str) \
{ \
	assert(str); \
	if (! entries || entrieslen == 0) { \
		return NULL; \
	} \
	uint32_t bucket_idx = fnv1a_hash(str, strlen(str)) % entrieslen; \
	uint32_t current = bucket_idx; \
	struct n##_hash_entry *found = NULL; \
	do { \
		assert(current >= 0); \
		assert(current < entrieslen); \
		found = &(entries[current]); \
		if (found->key == NULL || found->key == TABLE_SENTINEL_KEY) { \
			return found; \
		} \
		current = (current + 1) % entrieslen; \
	} while (current != bucket_idx); \
	return NULL; \
} \
\
/* Ensure the hasstruct nle has the given size. */ \
static int \
n##_HASH_GROW(struct n *tab, size_t newsize) \
{ \
	struct n##_hash_entry *newentries; \
	if (! tab) { \
		return TABLE_STATUS_ERROR; \
	} \
	if (newsize < (tab)->available) { \
		return TABLE_STATUS_OK; \
	} \
	newentries = calloc(newsize, sizeof(struct n##_hash_entry)); \
	struct n##_hash_entry *p, *newp; \
	for (size_t i = 0; i < (tab)->available; i++) { \
		p = &((tab)->entries[i]); \
		if (p->key == NULL || p->key == TABLE_SENTINEL_KEY) { \
			continue; \
		} \
		newp = n##_HASH_FIND_EMPTY_ENTRY(newentries, newsize, p->key); \
		assert(newp); /* This should never be null */ \
		newp->key = p->key; \
		newp->val = p->val; \
	} \
	free((tab)->entries); \
	(tab)->entries = newentries; \
	(tab)->available = newsize; \
	return TABLE_STATUS_OK; \
} \
 \
/* Find a value by a key. */ \
c *\
n##_HASH_FIND(struct n *h, char *str) \
{ \
	if (! h) { \
		return NULL; \
	} \
	struct n##_hash_entry *found = n##_HASH_FIND_FILLED_ENTRY(h->entries, h->available, str); \
	if (! found || ! found->key) { \
		return NULL; \
	} \
	return found->val; \
} \
 \
int \
n##_HASH_ENTER(struct n *h, char *str, c *item) \
{ \
	if (! h) { \
		return TABLE_STATUS_ERROR; \
	} \
	struct n##_hash_entry *found = n##_HASH_FIND_EMPTY_ENTRY(h->entries, h->available, str); \
	if (! found) { \
		if (h->available == 0) { \
			n##_HASH_GROW(h, 8); \
		} else { \
			n##_HASH_GROW(h, h->available * 2); \
		} \
		found = n##_HASH_FIND_EMPTY_ENTRY(h->entries, h->available, str); \
	} \
	assert(found); \
	found->key = str; \
	found->val = item; \
	h->count++; \
	return TABLE_STATUS_OK; \
} \
\
int \
n##_HASH_REMOVE(struct n *h, char *str) \
{ \
	if (! h) { \
		return TABLE_STATUS_ERROR; \
	} \
	struct n##_hash_entry *found = n##_HASH_FIND_FILLED_ENTRY(h->entries, h->available, str); \
	if (! found) { \
		return TABLE_STATUS_OK; \
	} \
	assert(found); \
	found->key = TABLE_SENTINEL_KEY; \
	found->val = NULL; \
	h->count--; \
	return TABLE_STATUS_OK; \
}

#define TABLE_FIND(name, tab, key)		 name##_HASH_FIND((tab), (key))
#define TABLE_ENTER(name, tab, key, val) name##_HASH_ENTER(tab, key, val)
#define TABLE_REMOVE(name, tab, key)	 name##_HASH_REMOVE(tab, key)

#endif // HASH_H

