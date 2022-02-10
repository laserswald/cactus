#ifndef __CACT_BOOL_H__
#define __CACT_BOOL_H__

#include "val.h"

DEFINE_VALUE_CONV(CACT_TYPE_BOOL, bool,   cact_to_bool,   boolean)
DEFINE_VALUE_CHECK(cact_is_bool,   CACT_TYPE_BOOL)

struct cact_val cact_bool_not(struct cact_val x);
bool cact_is_truthy(struct cact_val);

#endif /* __CACT_BOOL_H__ */
