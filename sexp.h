#ifndef sexp_h_INCLUDED
#define sexp_h_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "env.h"

typedef struct sexp cact_val;

typedef enum {
    TYPE_UNDEF,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_SYMBOL,
    TYPE_PAIR,
    TYPE_PROCEDURE,
    TYPE_ENVIRONMENT,
    TYPE_PORT,
    TYPE_ERROR,
} cact_type;

typedef struct {
    char *str;
} cact_string;

typedef struct {
    char* sym;
} cact_symbol;

typedef struct {
    cact_val *car;
    cact_val *cdr;
} cact_pair;

typedef struct {
    cact_env *env;
    cact_val *argl;
    cact_val *body;
    cact_val *(*nativefn)(cact_val *args, cact_env *e);
} cact_proc;

typedef struct {
    char *msg;
    cact_val *ctx;
} Error;

struct sexp {
    cact_type t;
    union {
        int i;
        double f;
        bool b;
        cact_string s;
        cact_symbol a; // atom
        cact_pair p;
        cact_proc *c;
        cact_env *e;
        Error x;
    };
};

cact_val* cons(cact_val* a, cact_val* d);
cact_val* car(cact_val* x);
cact_val* cdr(cact_val* x);
#define cadr(x) 		car(cdr(x))
#define caddr(x) 		car(cdr(cdr(x)))

cact_val* append(cact_val *l, cact_val *x);
cact_val* acons(cact_val* k, cact_val* v, cact_val* alist);
cact_val* assoc(cact_val* k, cact_val* alist);

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
GENERATE_TYPECONV(TYPE_PROCEDURE, cact_proc*, to_procedure, c)
GENERATE_TYPECONV(TYPE_BOOLEAN, bool, to_bool, b)
GENERATE_TYPECONV(TYPE_DOUBLE, double, to_float, f)
GENERATE_TYPECONV(TYPE_ENVIRONMENT, cact_env*, to_env, e)
GENERATE_TYPECONV(TYPE_INT, int, to_int, i)
GENERATE_TYPECONV(TYPE_PAIR, cact_pair, to_pair, p)
GENERATE_TYPECONV(TYPE_STRING, cact_string, to_str, s)
GENERATE_TYPECONV(TYPE_SYMBOL, cact_symbol, to_sym, a)

GENERATE_TYPECHECK(is_procedure, TYPE_PROCEDURE)
GENERATE_TYPECHECK(is_bool, TYPE_BOOLEAN)
GENERATE_TYPECHECK(is_env, TYPE_ENVIRONMENT)
GENERATE_TYPECHECK(is_float, TYPE_DOUBLE)
GENERATE_TYPECHECK(is_int, TYPE_INT)
GENERATE_TYPECHECK(is_pair, TYPE_PAIR)
GENERATE_TYPECHECK(is_str, TYPE_STRING)
GENERATE_TYPECHECK(is_sym, TYPE_SYMBOL)
GENERATE_TYPECHECK(is_error, TYPE_ERROR)

bool is_nil(cact_val *);
bool is_truthy(cact_val *);
cact_val *sexp_not(cact_val *x);

bool equals(cact_val *l, cact_val *r);

void print_sexp(cact_val *x);
void print_env(cact_env *e);
void print_list(cact_val *x);

cact_val *make_integer(int i);
cact_val *make_procedure(cact_env *e, cact_val *args, cact_val *body);
cact_val *make_error(char *msg, cact_val *irr);
cact_val *make_string(char *str);
cact_val *make_boolean(bool b);

#define LIST_FOR_EACH(list, current) \
for (cact_val *(current) = list;\
     current && is_pair(current); \
     current = cdr(current))

#define PROPAGATE_ERROR(err) if (is_error(err)) return (err);

extern cact_val undefined;

#endif // sexp_h_INCLUDED

