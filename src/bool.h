#ifndef __CACT_BOOL_H__
#define __CACT_BOOL_H__

#include "val.h"

DEFINE_VALUE_CONV(CACT_TYPE_BOOL, bool,   cact_to_bool,   boolean)
DEFINE_VALUE_CHECK(cact_is_bool, CACT_TYPE_BOOL)

cact_value_t cact_bool_not(cact_value_t x);
bool cact_is_truthy(cact_value_t);

#endif /* __CACT_BOOL_H__ */
