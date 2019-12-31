#ifndef BITSET_H
#define BITSET_H

#include <stdint.h>
#include "array.h"

ARRAY_DECL(bitset, uint64_t);

void bsinit(struct bitset *);
void bsset(struct bitset *, size_t);
void bsclr(struct bitset *, size_t);
int bsget(struct bitset *, size_t);
void bsand(struct bitset *, struct bitset *);
void bsor(struct bitset *, struct bitset *);
size_t bscnt(struct bitset *);
void bsfree(struct bitset *);

#endif // BITSET_H

