#ifndef __ENV_H__
#define __ENV_H__

#include "val.h"
#include "storage/obj.h"
#include "sym.h"

#include "internal/table.h"

TABLE_DECL(cact_env_entries, cact_symbol_t *, cact_value_t)
typedef struct cact_env_entries cact_env_entries_t;

typedef struct cact_env {
	cact_object_t obj;
	struct cact_env *parent;
	cact_env_entries_t entries;
} cact_env_t;

DEFINE_OBJECT_CONVERSION(CACT_OBJ_ENVIRONMENT, cact_env_t *, cact_to_env, env)
DEFINE_OBJECT_CHECK(cact_is_env, CACT_OBJ_ENVIRONMENT)

cact_value_t 
cact_make_env(cact_context_t *, cact_env_t *parent);

void 
cact_env_init(cact_env_t *e, cact_env_t *parent);

void 
cact_mark_env(cact_object_t *);

void 
cact_destroy_env(cact_object_t *);

cact_value_t 
cact_env_define(cact_context_t *, cact_env_t *e, cact_symbol_t *k, cact_value_t v);

cact_value_t 
cact_env_set(cact_context_t *, cact_env_t *e, cact_symbol_t *k, cact_value_t v);

cact_value_t 
cact_env_lookup(cact_context_t *, cact_env_t *e, cact_symbol_t *k);

bool 
cact_env_is_bound(cact_env_t *e, cact_symbol_t *k);

void 
print_env(cact_env_t *e);

int
cact_env_num_bindings(cact_env_t *e);

#endif // env_h_INCLUDED

