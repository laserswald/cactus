#ifndef CACT_PAIR_H
#define CACT_PAIR_H

#include "cactus/core.h"

#include "cactus/obj.h"
#include "cactus/val.h"

struct cact_pair {
	struct cact_obj obj;
	struct cact_val car;
	struct cact_val cdr;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PAIR,        struct cact_pair*,   cact_to_pair,      pair)
DEFINE_OBJECT_CHECK(cact_is_pair, CACT_OBJ_PAIR)

struct cact_val cact_cons(struct cactus*, struct cact_val, struct cact_val);
struct cact_val cact_car(struct cactus *, struct cact_val);
struct cact_val cact_cdr(struct cactus *,struct cact_val);
struct cact_val cact_set_car(struct cactus *, struct cact_val, struct cact_val);
struct cact_val cact_set_cdr(struct cactus *, struct cact_val, struct cact_val);

void cact_mark_pair(struct cact_obj *);
void cact_destroy_pair(struct cact_obj *);

#define cact_cadr(cact, x) 	    cact_car(cact, cact_cdr(cact, x))
#define cact_caddr(cact, x) 	cact_car(cact, cact_cdr(cact, cact_cdr(cact, x)))

struct cact_val cact_append(struct cactus *, struct cact_val l, struct cact_val x);
struct cact_val cact_list_acons(struct cactus *, struct cact_val k, struct cact_val v, struct cact_val alist);
struct cact_val cact_assoc(struct cactus *, struct cact_val k, struct cact_val alist);
unsigned int cact_length(struct cactus *, struct cact_val);

/*
 * Iterate through an entire list, updating `pair` to point to the current
 * element.
 */
#define CACT_LIST_FOR_EACH_PAIR(cact, pair, list) \
    struct cact_val *pair;\
    for (pair = &list; \
         cact_is_pair(*pair); \
         pair = &((struct cact_pair *) pair->as.object)->cdr)

#define CACT_LIST_FOR_EACH_ITEM(cact, item, list) \
    struct cact_val *__list_item_pair__; \
    struct cact_val item; \
    for (__list_item_pair__ = &list, item = cact_car(cact, *__list_item_pair__); \
         \
         cact_is_pair(*__list_item_pair__); \
         \
         __list_item_pair__ = &((struct cact_pair *) __list_item_pair__->as.object)->cdr, \
         item = cact_is_pair(*__list_item_pair__) \
            ? cact_car(cact, *__list_item_pair__) \
            : CACT_NULL_VAL) \



#endif /* CACT_PAIR_H */
