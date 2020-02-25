#ifndef sexp_h_INCLUDED
#define sexp_h_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "sym.h"

struct cactus;

typedef struct cact_env cact_env;
typedef struct cact_val cact_val;

typedef enum {
    CACT_TYPE_NULL,
    CACT_TYPE_INT,
    CACT_TYPE_DOUBLE,
    CACT_TYPE_BOOLEAN,
    CACT_TYPE_CHARACTER,
    CACT_TYPE_OBJECT,
    CACT_TYPE_ERROR,
} cact_type;

struct cact_val {
    cact_type t;
    union {
        int i;
        double f;
        bool b;
        char c;
        struct cact_obj *obj;
        Error x;
    };
};

#define CACT_NULL ((cact_val) {CACT_TYPE_NULL, 0})
#define CACT_INT_VAL(n) ((cact_val) {CACT_TYPE_INT, .i = (n)})
#define CACT_DOUBLE_VAL(n) ((cact_val) {CACT_TYPE_DOUBLE, .d = (n)})
#define CACT_BOOLEAN_VAL(p) ((cact_val) {CACT_TYPE_BOOLEAN, .b = (p)})
#define CACT_BOOLEAN_VAL(p) ((cact_val) {CACT_TYPE_CHARACTER, .c = (p)})

bool cact_is_null(struct cact_val);

const char *show_type(cact_type t);
const char *show_sexp(cact_val *x);

#define GENERATE_TYPECONV(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(cact_val *x, char *extras) \
{ \
 	if (!x) { \
		fprintf(stderr, "%s: Expected %s, but got nil.\n", extras, show_type(typemarker)); \
		abort(); \
 	} \
	if (x->t != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, show_type(typemarker), show_type(x->t)); \
		abort(); \
	} \
	return x->membername; \
}

#define GENERATE_TYPECHECK(funcname, typemarker) \
static inline bool \
funcname(cact_val *x) { \
	return x && x->t == typemarker; \
}

/* generated functions */
GENERATE_TYPECONV(CACT_TYPE_BOOLEAN, bool, to_bool, b)
GENERATE_TYPECONV(CACT_TYPE_DOUBLE, double, to_float, f)
GENERATE_TYPECONV(CACT_TYPE_INT, int, to_int, i)
GENERATE_TYPECONV(CACT_TYPE_SYMBOL, struct cact_symbol *, to_sym, a)

GENERATE_TYPECHECK(cact_is_bool, CACT_TYPE_BOOLEAN)
GENERATE_TYPECHECK(cact_is_float, CACT_TYPE_DOUBLE)
GENERATE_TYPECHECK(cact_is_int, CACT_TYPE_INT)

bool cact_is_truthy(struct cact_val);
struct cact_val cact_not(struct cact_val);

bool cact_val_eq(struct cact_val l, struct cact_val r);
bool cact_val_eqv(struct cact_val l, struct cact_val r);
bool cact_val_equal(struct cact_val l, struct cact_val r);

struct cact_val cact_make_procedure(cact_env *e, cact_val *args, cact_val *body);
struct cact_val cact_make_error(char *msg, cact_val *irr);
struct cact_val cact_make_string(char *str);

#define PROPAGATE_ERROR(err) if (is_error(err)) return (err);

extern cact_val undefined;

#endif // sexp_h_INCLUDED

