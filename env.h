#ifndef __ENV_H__
#define __ENV_H__

typedef struct env Env;
typedef struct sexp Sexp;

#include "sexp.h"

struct env {
    Env *parent;
    Sexp *list;
};

void envinit(Env *e, Env *parent);
int envadd(Env *e, Sexp *k, Sexp *v);
int envset(Env *e, Sexp *k, Sexp *v);
Sexp *envlookup(Env *e, Sexp *k);
void print_env(Env *e);

#endif // env_h_INCLUDED

