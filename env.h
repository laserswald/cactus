#ifndef __ENV_H__
#define __ENV_H__

#include "sexp.h"

struct cact_env {
    struct cact_env *parent;
    struct cact_val *list;
};

void envinit(struct cact_env *e, struct cact_env *parent);
int envadd(struct cact_env *e, struct cact_val *k, struct cact_val *v);
int envset(struct cact_env *e, struct cact_val *k, struct cact_val *v);
struct cact_val *envlookup(struct cact_env *e, struct cact_val *k);
void print_env(struct cact_env *e);

#endif // env_h_INCLUDED

