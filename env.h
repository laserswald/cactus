#ifndef __ENV_H__
#define __ENV_H__

typedef struct env cact_env;
typedef struct sexp cact_val;

#include "sexp.h"

struct env {
    cact_env *parent;
    cact_val *list;
};

void envinit(cact_env *e, cact_env *parent);
int envadd(cact_env *e, cact_val *k, cact_val *v);
int envset(cact_env *e, cact_val *k, cact_val *v);
cact_val *envlookup(cact_env *e, cact_val *k);
void print_env(cact_env *e);

#endif // env_h_INCLUDED

