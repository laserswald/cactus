#ifndef CACT_CHAR_H
#define CACT_CHAR_H

#include "val.h"

DEFINE_VALUE_CONV(CACT_TYPE_CHAR, char, cact_to_char, character)
DEFINE_VALUE_CHECK(cact_is_char,  CACT_TYPE_CHAR)

#endif // CACT_CHAR_H

