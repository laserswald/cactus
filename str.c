#include "str.h"

/* Create a string. */
cact_val *
cact_make_string(char *str)
{
    cact_val *x = malloc(sizeof(cact_val));
    x->t = CACT_TYPE_STRING;
    x->s.str = str;
    return x;
}

