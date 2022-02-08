#include "cactus/syn.h"

#include "cactus/bool.h"
#include "cactus/num.h"
#include "cactus/sym.h"
#include "cactus/pair.h"
#include "cactus/str.h"
#include "cactus/err.h"
#include "cactus/env.h"

/*
 * Syntax recognizers.
 */

bool is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x);

bool
is_self_evaluating(struct cact_val x)
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
is_variable(struct cactus *cact, struct cact_val x)
{
    return cact_is_symbol(x);
}

bool
is_quotation(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "quote", x) || is_tagged_pair(cact, "quasiquote", x);
}

bool
is_conditional(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "if", x);
}

bool
is_definition(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "define", x);
}

bool
is_lambda(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "lambda", x);
}

bool
is_sequence(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "begin", x);
}

bool
is_assignment(struct cactus *cact, struct cact_val x)
{
    return is_tagged_pair(cact, "set!", x);
}

bool
is_application(struct cactus *cact, struct cact_val x)
{
    return cact_is_pair(x);
}

bool
is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x)
{
    if (! cact_is_pair(x)) {
        return false;
    }

    struct cact_val operator = cact_car(cact, x);
    struct cact_symbol *tagsym = cact_get_symbol(cact, tag);

    if (! cact_is_symbol(operator)) {
        return false;
    }

    struct cact_symbol *opsym = cact_to_symbol(operator, "is_tagged_pair");

    return tagsym == opsym;
}

