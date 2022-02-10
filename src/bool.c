#include "bool.h"

/*
 * Return true if this value is true according to an 
 * if expression. 
 * 
 * That is, not #f.
 */
bool
cact_is_truthy(const struct cact_val x)
{
    return ! cact_is_bool(x) || cact_to_bool(x, "truthy?"); // adsfkja;dslkjf;lkdsaj
}

/*
 * Return #t if the value is #f, and #f if the value is otherwise.
 */
struct cact_val
cact_bool_not(const struct cact_val x)
{
    return CACT_BOOL_VAL(cact_is_bool(x) && x.as.boolean == false);
}
