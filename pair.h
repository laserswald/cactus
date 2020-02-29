#ifndef CACT_PAIR_H
#define CACT_PAIR_H

#include "sexp.h"
#include "obj.h"

typedef struct cact_pair {
	struct cact_val *car;
	struct cact_val *cdr;
} cact_pair;

struct cact_val
cact_make_pair(struct cactus*, struct cact_val, struct cact_val);

struct cact_val
cact_car(struct cact_val);

struct cact_val
cact_cdr(struct cact_val);

#define cact_cadr(x) 	cact_car(cact_cdr(x))
#define cact_caddr(x) 	cact_car(cact_cdr(cact_cdr(x)))

void
cact_set_car(struct cact_val, struct cact_val);

void
cact_set_cdr(struct cact_val, struct cact_val);

struct cact_val 
append(struct cact_val l, struct cact_val x);

struct cact_val
acons(struct cact_val k, struct cact_val* v, struct cact_val* alist);

struct cact_val* 
assoc(struct cact_val* k, struct cact_val* alist);

#endif /* CACT_PAIR_H */
