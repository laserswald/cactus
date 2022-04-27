#ifndef CACT_PAIR_H
#define CACT_PAIR_H

#include "core.h"

#include "storage/obj.h"
#include "val.h"

typedef struct cact_pair {
	cact_object_t obj;
	cact_value_t car;
	cact_value_t cdr;
} cact_pair_t;

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PAIR, cact_pair_t*, cact_to_pair, pair)
DEFINE_OBJECT_CHECK(cact_is_pair, CACT_OBJ_PAIR)

cact_value_t 
cact_cons(cact_context_t *cact, cact_value_t a, cact_value_t d);

cact_value_t 
cact_car(cact_context_t *cact, cact_value_t pair);

cact_value_t 
cact_cdr(cact_context_t *cact, cact_value_t pair);

cact_value_t 
cact_set_car(cact_context_t *cact, cact_value_t pair, cact_value_t v);

cact_value_t 
cact_set_cdr(cact_context_t *cact, cact_value_t pair, cact_value_t v);

void cact_mark_pair(cact_object_t *);
void cact_destroy_pair(cact_object_t *);

#define cact_cadr(cact, x) 	    cact_car(cact, cact_cdr(cact, x))
#define cact_caddr(cact, x) 	cact_car(cact, cact_cdr(cact, cact_cdr(cact, x)))

cact_value_t 
cact_append(cact_context_t *cact, cact_value_t l, cact_value_t x);

cact_value_t 
cact_list_acons(cact_context_t *cact, cact_value_t k, cact_value_t v, cact_value_t alist);

cact_value_t 
cact_assoc(cact_context_t *cact, cact_value_t k, cact_value_t alist);

unsigned int 
cact_length(cact_context_t *cact, cact_value_t l);

/*
 * Iterate through an entire list, updating `pair` to point to the current
 * element.
 */
#define CACT_LIST_FOR_EACH_PAIR(cact, pair, list) \
    cact_value_t *pair;\
    for (pair = &list; \
         cact_is_pair(*pair); \
         pair = &((cact_pair_t *) pair->as.object)->cdr)

#define CACT_LIST_FOR_EACH_ITEM(cact, item, list) \
    cact_value_t *__list_item_pair__; \
    cact_value_t item; \
    for (__list_item_pair__ = &list, item = cact_car(cact, *__list_item_pair__); \
         \
         cact_is_pair(*__list_item_pair__); \
         \
         __list_item_pair__ = &((cact_pair_t *) __list_item_pair__->as.object)->cdr, \
         item = cact_is_pair(*__list_item_pair__) \
            ? cact_car(cact, *__list_item_pair__) \
            : CACT_NULL_VAL) \



#endif /* CACT_PAIR_H */
