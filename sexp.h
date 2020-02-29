#ifndef sexp_h_INCLUDED
#define sexp_h_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "sym.h"
#include "pair.h"

struct cactus;

typedef struct cact_env cact_env;
typedef struct cact_val cact_val;

typedef enum cact_type {
    CACT_TYPE_NULL,
    CACT_TYPE_FIXNUM,
    CACT_TYPE_FLONUM,
    CACT_TYPE_BOOL,
    CACT_TYPE_CHAR,
    CACT_TYPE_OBJ,
} cact_type;

struct cact_val {
    enum cact_type type;
    union {
        long      fixnum;
        double    flonum;
        bool      boolean;
        char      character;
        struct cact_obj *object;
    } as;
};

#define CACT_NULL_VAL    ((cact_val) {CACT_TYPE_NULL, 0})
#define CACT_FIX_VAL(n)  ((cact_val) {CACT_TYPE_FIXNUM,    .fixnum = (n)})
#define CACT_FLO_VAL(n)  ((cact_val) {CACT_TYPE_FLONUM,    .flonum = (n)})
#define CACT_BOOL_VAL(p) ((cact_val) {CACT_TYPE_BOOLEAN,   .boolean = (p)})
#define CACT_CHAR_VAL(p) ((cact_val) {CACT_TYPE_CHARACTER, .character = (p)})
#define CACT_OBJ_VAL(p)  ((cact_val) {CACT_TYPE_OBJ,       .object = (p)})

bool cact_is_null(struct cact_val);

const char *show_type(cact_type t);
const char *show_sexp(cact_val *x);

#define DEFINE_VALUE_CONV(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(cact_val x, char *extras) \
{ \
	if (x.type != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, show_type(typemarker), show_type(x.type)); \
		abort(); \
	} \
	return x.as.membername; \
}

#define DEFINE_VALUE_CHECK(funcname, typemarker) \
static inline bool \
funcname(cact_val x) { \
	return x.type == typemarker; \
}

/* generated functions */
DEFINE_VALUE_CONV(CACT_TYPE_BOOL, bool,   cact_to_bool,   boolean)
DEFINE_VALUE_CONV(CACT_TYPE_FLONUM,  double, cact_to_double, flonum)
DEFINE_VALUE_CONV(CACT_TYPE_FIXNUM,  int,    cact_to_long,   fixnum)
DEFINE_VALUE_CONV(CACT_TYPE_CHAR,  char,cact_to_char,   character)
DEFINE_VALUE_CONV(CACT_TYPE_OBJ, struct cact_obj*, cact_to_obj,   object)

DEFINE_VALUE_CHECK(cact_is_bool,   CACT_TYPE_BOOL)
DEFINE_VALUE_CHECK(cact_is_flonum, CACT_TYPE_FLONUM)
DEFINE_VALUE_CHECK(cact_is_fixnum, CACT_TYPE_FIXNUM)
DEFINE_VALUE_CHECK(cact_is_char, CACT_TYPE_CHAR)
DEFINE_VALUE_CHECK(cact_is_obj, CACT_TYPE_OBJ)

bool cact_is_truthy(struct cact_val);
struct cact_val cact_not(struct cact_val);

bool cact_val_eq(struct cact_val l, struct cact_val r);
bool cact_val_eqv(struct cact_val l, struct cact_val r);
bool cact_val_equal(struct cact_val l, struct cact_val r);

struct cact_val cact_make_error(char *msg, cact_val *irr);

#define PROPAGATE_ERROR(err) if (is_error(err)) return (err);

#endif // sexp_h_INCLUDED

