#ifndef CACT_PAIR_H
#define CACT_PAIR_H

#include "cactus/val.h"

typedef struct cact_pair {
	struct cact_val car;
	struct cact_val cdr;
} cact_pair;

struct cact_val
cact_make_pair(struct cactus*, struct cact_val, struct cact_val);

struct cact_val
cact_car(struct cactus *, struct cact_val);

struct cact_val
cact_cdr(struct cactus *,struct cact_val);

#define cact_cadr(x) 	cact_car(cact_cdr(x))
#define cact_caddr(x) 	cact_car(cact_cdr(cact_cdr(x)))

void
cact_set_car(struct cact_val, struct cact_val);

void
cact_set_cdr(struct cact_val, struct cact_val);

struct cact_val 
cact_append(struct cactus*, struct cact_val l, struct cact_val x);

struct cact_val
cact_acons(struct cactus *, struct cact_val k, struct cact_val v, struct cact_val alist);

struct cact_val
cact_assoc(struct cact_val k, struct cact_val alist);

unsigned int
cact_length(struct cact_val);

#define CACT_LIST_FOR_EACH_PAIR(pair, list) \
    struct cact_val pair;\
    for (pair = list; cact_is_pair(pair); pair = cact_cdr(list))

#define CACT_LIST_FOR_EACH_ITEM(item, list) \
    struct cact_val __list_item_pair__; \
    struct cact_val item; \
    for (__list_item_pair__ = list, item = cact_car(__list_item_pair__); \
         \
         cact_is_pair(__list_item_pair__); \
         \
         __list_item_pair__ = cact_cdr(list), \
         item = cact_is_pair(__list_item_pair__) \
            ? cact_car(__list_item_pair__) \
            : CACT_NULL_VAL) \


#endif /* CACT_PAIR_H */
