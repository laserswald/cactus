#include <assert.h>
#include "utils.h"
#include "bitset.h"

void 
bsinit(struct bitset *bs)
{
	ARRAY_INIT(bs);
}

uint64_t 
bsgethunk(struct bitset *bs, size_t point)
{
    size_t hunkidx = point / 64;

    uint64_t hunk;

    if (ARRAY_LENGTH(bs) <= hunkidx) {
        hunk = 0;
    } else {
	    hunk = ARRAY_ITEM(bs, hunkidx);
    }

    return hunk;
}

size_t
bscounthunk(uint64_t hunk)
{
	int i;
	size_t cnt;
	for (i = 0; i < 64; i++) {
		if (hunk & (1 << i)) {
			cnt++;
		}
	}
	return cnt;
}

void 
bsset(struct bitset *bs, size_t point)
{
    uint64_t hunk = bsgethunk(bs, point);
    size_t bitidx = point % 64;
    size_t hunkidx = point / 64;
    hunk |= 1 << bitidx;
    ARRAY_SET(bs, hunkidx, hunk);
}

void 
bsclr(struct bitset *bs, size_t point)
{
    uint64_t hunk = bsgethunk(bs, point);
    size_t bitidx = point % 64;
    size_t hunkidx = point / 64;
    hunk &= 0 << bitidx;
    ARRAY_SET(bs, hunkidx, hunk);
}

int 
bsget(struct bitset *bs, size_t point)
{
    uint64_t hunk = bsgethunk(bs, point);
    return (hunk >> (point % 64)) & 1;
}

void
bsand(struct bitset *a, struct bitset *b)
{
	int minlen = MIN(ARRAY_LENGTH(a), ARRAY_LENGTH(b)); 
	int maxlen = MAX(ARRAY_LENGTH(a), ARRAY_LENGTH(b)); 
	int i;
	for (i = 0; i < minlen; i++) {
        uint64_t ahunk = ARRAY_ITEM(a, i);
        uint64_t bhunk = ARRAY_ITEM(b, i);

        ahunk &= bhunk;
        ARRAY_SET(a, i, ahunk);
	}
	assert(i == minlen - 1);
	for (; i < maxlen; i++) {
        ARRAY_SET(a, i, 0);
	}
}

void
bsor(struct bitset *a, struct bitset *b)
{
	int minlen = MIN(ARRAY_LENGTH(a), ARRAY_LENGTH(b)); 
	int maxlen = MAX(ARRAY_LENGTH(a), ARRAY_LENGTH(b)); 
	int i;
	for (i = 0; i < minlen; i++) {
        uint64_t ahunk = ARRAY_ITEM(a, i);
        uint64_t bhunk = ARRAY_ITEM(b, i);

        ahunk |= bhunk;
        ARRAY_SET(a, i, ahunk);
	}
	assert(i == minlen - 1);
	for (; i < maxlen; i++) {
        ARRAY_SET(a, i, 0);
	}
}

size_t 
bscnt(struct bitset *bs)
{
    size_t cnt = 0;
    int i;
    for (i = 0; i < ARRAY_LENGTH(bs); i++) {
	    cnt += bscounthunk(bsgethunk(bs, i));
    }
    return cnt;
}

void 
bsfree(struct bitset *bs)
{
    ARRAY_FREE(bs);
}
