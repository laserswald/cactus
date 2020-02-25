#ifndef pair_h_INCLUDED
#define pair_h_INCLUDED

#include "obj.h"

/* A pair of values. The myth. The legend. */
struct cact_pair {
    struct cact_obj obj;
    struct cact_val car;
    struct cact_val cdr;
};

/* Construct a new pair. */
struct cact_val cact_cons(struct cactus *, struct cact_val, struct cact_val);

/* Get the car of the pair, or an error if the value is not a pair. */
struct cact_val cact_car(struct cact_val);

/* Get the cdr of the pair, or an error if the value is not a pair. */
struct cact_val cact_cdr(struct cact_val);

#define cact_cadr(x) 		cact_car(cact_cdr(x))
#define cact_caddr(x) 		cact_car(cact_cdr(cact_cdr(x)))

cact_val* append(cact_val *l, cact_val *x);
cact_val* acons(cact_val* k, cact_val* v, cact_val* alist);
cact_val* assoc(cact_val* k, cact_val* alist);

#define LIST_FOR_EACH(list, current) \
for (cact_val *(current) = list;\
     current && is_pair(current); \
     current = cdr(current))

#endif // pair_h_INCLUDED

