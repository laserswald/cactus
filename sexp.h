#ifndef sexp_h_INCLUDED
#define sexp_h_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "env.h"

typedef struct sexp Sexp;

typedef enum {
	TYPE_UNDEF,
	TYPE_INT,
	TYPE_DOUBLE,
	TYPE_BOOLEAN,
	TYPE_STRING,
	TYPE_SYMBOL,
	TYPE_PAIR,
	TYPE_CLOSURE,
	TYPE_ENVIRONMENT,
	TYPE_PORT,
	TYPE_ERROR,
} Type;

typedef struct {
	char *str;
} String;

typedef struct {
	char* sym;
} Symbol;

typedef struct {
	Sexp *car;
	Sexp *cdr;
} Pair;

typedef struct {
	Env *env;
	Sexp *argl;
	Sexp *body;
	Sexp *(*nativefn)(Sexp *args, Env *e);
} Closure;

typedef struct {
	char *msg;
	Sexp *ctx;
} Error;

struct sexp {
	Type t;
	union {
		int i;
		double f;
		bool b;
		String s;
		Symbol a; // atom
		Pair p;
		Closure *c;
		Env *e;
		Error x;
	};
};

Sexp* cons(Sexp* a, Sexp* d);
Sexp* car(Sexp* x);
Sexp* cdr(Sexp* x);
#define cadr(x) 		car(cdr(x))
#define caddr(x) 		car(cdr(cdr(x)))

Sexp* append(Sexp *l, Sexp *x);
Sexp* acons(Sexp* k, Sexp* v, Sexp* alist);
Sexp* assoc(Sexp* k, Sexp* alist);

const char *show_type(Type t);
const char *show_sexp(Sexp *x);

#define GENERATE_TYPECONV(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(Sexp *x, char *extras) \
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
funcname(Sexp *x) { \
	return x && x->t == typemarker; \
}

/* generated functions */
GENERATE_TYPECONV(TYPE_CLOSURE, Closure*, to_closure, c)
GENERATE_TYPECONV(TYPE_BOOLEAN, bool, to_bool, b)
GENERATE_TYPECONV(TYPE_DOUBLE, double, to_float, f)
GENERATE_TYPECONV(TYPE_ENVIRONMENT, Env*, to_env, e)
GENERATE_TYPECONV(TYPE_INT, int, to_int, i)
GENERATE_TYPECONV(TYPE_PAIR, Pair, to_pair, p)
GENERATE_TYPECONV(TYPE_STRING, String, to_str, s)
GENERATE_TYPECONV(TYPE_SYMBOL, Symbol, to_sym, a)

GENERATE_TYPECHECK(is_closure, TYPE_CLOSURE)
GENERATE_TYPECHECK(is_bool, TYPE_BOOLEAN)
GENERATE_TYPECHECK(is_env, TYPE_ENVIRONMENT)
GENERATE_TYPECHECK(is_float, TYPE_DOUBLE)
GENERATE_TYPECHECK(is_int, TYPE_INT)
GENERATE_TYPECHECK(is_pair, TYPE_PAIR)
GENERATE_TYPECHECK(is_str, TYPE_STRING)
GENERATE_TYPECHECK(is_sym, TYPE_SYMBOL)
GENERATE_TYPECHECK(is_error, TYPE_ERROR)

bool is_nil(Sexp *);
bool is_truthy(Sexp *);

bool equals(Sexp *l, Sexp *r);

void print_sexp(Sexp *x);
void print_env(Env *e);
void print_list(Sexp *x);

Sexp *make_integer(int i);
Sexp *make_closure(Env *e, Sexp *args, Sexp *body);
Sexp *make_error(char *msg, Sexp *irr);
Sexp *make_string(char *str);

#define LIST_FOR_EACH(list, current) \
for (Sexp *(current) = list;\
     current && is_pair(current); \
     current = cdr(current))

#define PROPAGATE_ERROR(err) if (is_error(err)) return (err);

extern Sexp undefined;

#endif // sexp_h_INCLUDED

