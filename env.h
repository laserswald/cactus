#ifndef __ENV_H__
#define __ENV_H__

#include "sexp.h"

struct cact_env {
    struct cact_env *parent;
    struct cact_val list;
};

void cact_env_init(struct cact_env *e, struct cact_env *parent);
int cact_env_define(struct cactus *, struct cact_env *e, struct cact_val k, struct cact_val v);
int cact_env_set(struct cact_env *e, struct cact_val k, struct cact_val v);
struct cact_val cact_env_lookup(struct cact_env *e, struct cact_val k);
void print_env(struct cact_env *e);

#endif // env_h_INCLUDED

