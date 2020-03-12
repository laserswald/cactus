#ifndef sexp_h_INCLUDED
#define sexp_h_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct cactus;

enum cact_type {
	CACT_TYPE_UNDEF,
    CACT_TYPE_NULL,
    CACT_TYPE_FIXNUM,
    CACT_TYPE_FLONUM,
    CACT_TYPE_BOOL,
    CACT_TYPE_CHAR,
    CACT_TYPE_OBJ,
};

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

#define CACT_NULL_VAL    ((struct cact_val){.type=CACT_TYPE_NULL})
#define CACT_UNDEF_VAL   ((struct cact_val){.type=CACT_TYPE_UNDEF})
#define CACT_FIX_VAL(n)  ((struct cact_val){.type=CACT_TYPE_FIXNUM, .as.fixnum = (n)})
#define CACT_FLO_VAL(n)  ((struct cact_val){.type=CACT_TYPE_FLONUM, .as.flonum = (n)})
#define CACT_BOOL_VAL(p) ((struct cact_val){.type=CACT_TYPE_BOOL, .as.boolean = (p)})
#define CACT_CHAR_VAL(p) ((struct cact_val){.type=CACT_TYPE_CHAR, .as.character = (p)})
#define CACT_OBJ_VAL(p)  ((struct cact_val){.type=CACT_TYPE_OBJ, .as.object = (p)})

const char *cact_val_show_type(enum cact_type t);
const char *cact_type_str(struct cact_val x);

#define DEFINE_VALUE_CONV(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(struct cact_val x, char *extras) \
{ \
	if (x.type != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, cact_val_show_type(typemarker), cact_type_str(x)); \
		abort(); \
	} \
	return x.as.membername; \
}

#define DEFINE_VALUE_CHECK(funcname, typemarker) \
static inline bool \
funcname(struct cact_val x) { \
	return x.type == typemarker; \
}

/* generated functions */
DEFINE_VALUE_CONV(CACT_TYPE_CHAR,  char,cact_to_char,   character)
DEFINE_VALUE_CONV(CACT_TYPE_OBJ, struct cact_obj*, cact_to_obj,   object)

DEFINE_VALUE_CHECK(cact_is_undef,   CACT_TYPE_UNDEF)
DEFINE_VALUE_CHECK(cact_is_char, CACT_TYPE_CHAR)
DEFINE_VALUE_CHECK(cact_is_obj, CACT_TYPE_OBJ)
bool cact_is_null(struct cact_val);

bool cact_val_eq(struct cact_val l, struct cact_val r);
bool cact_val_eqv(struct cact_val l, struct cact_val r);
bool cact_val_equal(struct cact_val l, struct cact_val r);

struct cact_val cact_make_error(struct cactus *, char *msg, struct cact_val irr);
#define PROPAGATE_ERROR(err) if (cact_is_error(err)) return (err);

#endif // sexp_h_INCLUDED

