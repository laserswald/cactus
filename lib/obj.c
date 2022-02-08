
#include "cactus/obj.h"

#include "cactus/pair.h"
#include "cactus/str.h"
#include "cactus/proc.h"
#include "cactus/env.h"
#include "cactus/err.h"
#include "cactus/vec.h"
#include "cactus/core.h"

const char* cact_obj_show_type(enum cact_obj_type t)
{
    switch (t) {
    case CACT_OBJ_PAIR:
        return "pair";
    case CACT_OBJ_STRING:
        return "string";
    case CACT_OBJ_PROCEDURE:
        return "procedure";
    case CACT_OBJ_ENVIRONMENT:
        return "environment";
    case CACT_OBJ_ERROR:
        return "error";
    case CACT_OBJ_CONT:
        return "continuation";
    case CACT_OBJ_VECTOR:
        return "vector";
    }
    return NULL;
}

struct {
    void (*mark_fn)(struct cact_obj *);
    void (*destroy_fn)(struct cact_obj *);
} object_gc_methods[] = {
    [CACT_OBJ_PAIR] = {cact_mark_pair, cact_destroy_pair},
    [CACT_OBJ_STRING] = {cact_mark_string, cact_destroy_string},
    [CACT_OBJ_PROCEDURE] = {cact_mark_proc, cact_destroy_proc},
    [CACT_OBJ_ENVIRONMENT] = {cact_mark_env, cact_destroy_env},
    [CACT_OBJ_CONT] = {cact_mark_cont, cact_destroy_cont},
    [CACT_OBJ_ERROR] = {cact_mark_error, cact_destroy_error},
    [CACT_OBJ_VECTOR] = {cact_mark_vec, cact_destroy_vec},
};

void
cact_obj_mark(struct cact_obj *obj)
{
	if (! obj) {
		return;
	}

    if (obj->store_data.mark == CACT_STORE_MARK_REACHABLE) {
        return;
    }

    obj->store_data.mark = CACT_STORE_MARK_REACHABLE;
    object_gc_methods[obj->type].mark_fn(obj);
}

void
cact_obj_destroy(struct cact_obj *obj)
{
	if (! obj) {
		return;
	}

    object_gc_methods[obj->type].destroy_fn(obj);
}

