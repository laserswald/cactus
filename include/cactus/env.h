#ifndef __ENV_H__
#define __ENV_H__

#include "cactus/val.h"
#include "cactus/obj.h"
#include "cactus/sym.h"

#include "cactus/internal/table.h"

TABLE_DECL(cact_env_entries, struct cact_symbol *, struct cact_val)

struct cact_env {
	struct cact_obj obj;
	struct cact_env *parent;
	struct cact_env_entries entries;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_ENVIRONMENT, struct cact_env*, cact_to_env, env)
DEFINE_OBJECT_CHECK(cact_is_env, CACT_OBJ_ENVIRONMENT)

struct cact_val cact_make_env(struct cactus *, struct cact_env *parent);
void            cact_env_init(struct cact_env *e, struct cact_env *parent);

void            cact_mark_env(struct cact_obj *);
void            cact_destroy_env(struct cact_obj *);

struct cact_val cact_env_define(struct cactus *, struct cact_env *e, struct cact_symbol *k, struct cact_val v);
struct cact_val cact_env_set(struct cactus *, struct cact_env *e, struct cact_symbol *k, struct cact_val v);
struct cact_val cact_env_lookup(struct cactus *, struct cact_env *e, struct cact_symbol *k);
bool cact_env_is_bound(struct cact_env *e, struct cact_symbol *k);

void            print_env(struct cact_env *e);


#endif // env_h_INCLUDED

