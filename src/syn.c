#include "syn.h"

#include "bool.h"
#include "num.h"
#include "sym.h"
#include "pair.h"
#include "str.h"
#include "err.h"
#include "env.h"

/*
 * Syntax recognizers.
 */

bool is_tagged_pair(cact_context_t *cact, const char* tag, cact_value_t x);

bool
is_self_evaluating(cact_value_t x)
{
    return cact_is_null(x)
           || cact_is_bool(x)
           || cact_is_number(x)
           || cact_is_procedure(x)
           || cact_is_string(x)
           || cact_is_error(x)
           || cact_is_env(x);
}

bool
is_variable(cact_context_t *cact, cact_value_t x)
{
    return cact_is_symbol(x);
}

bool
is_quotation(cact_context_t *cact, cact_value_t x)
{
    return is_tagged_pair(cact, "quote", x) || is_tagged_pair(cact, "quasiquote", x);
}

bool
is_conditional(cact_context_t *cact, cact_value_t x)
{
    return is_tagged_pair(cact, "if", x);
}

bool
is_definition(cact_context_t *cact, cact_value_t x)
{
    return is_tagged_pair(cact, "define", x);
}

bool
is_lambda(cact_context_t *cact, cact_value_t x)
{
    return is_tagged_pair(cact, "lambda", x);
}

bool
is_sequence(cact_context_t *cact, cact_value_t x)
{
    return is_tagged_pair(cact, "begin", x);
}

bool
is_assignment(cact_context_t *cact, cact_value_t x)
{
    return is_tagged_pair(cact, "set!", x);
}

bool
is_application(cact_context_t *cact, cact_value_t x)
{
    return cact_is_pair(x);
}

bool
is_tagged_pair(cact_context_t *cact, const char* tag, cact_value_t x)
{
    if (! cact_is_pair(x)) {
        return false;
    }

    cact_value_t operator = cact_car(cact, x);
    cact_symbol_t *tagsym = cact_get_symbol(cact, tag);

    if (! cact_is_symbol(operator)) {
        return false;
    }

    cact_symbol_t *opsym = cact_to_symbol(operator, "is_tagged_pair");

    return tagsym == opsym;
}

