#ifndef __CACT_VEC_H__
#define __CACT_VEC_H__

#include "cactus/obj.h"
#include "cactus/obj.h"

struct cact_vec {
    struct cact_obj obj;
    struct cact_val *items;
    size_t length;
}

struct cact_vec *cact_make_vec(struct cactus *cact, size_t length);

#endif // __CACT_VEC_H__
