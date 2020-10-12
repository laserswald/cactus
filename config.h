#ifndef CONFIG_H
#define CONFIG_H

#include "cactus/eval.h"
#include "cactus/builtin.h"

struct cact_builtin builtins[] = {
	// The special forms.

    {"car", cact_builtin_car},
    {"cdr", cact_builtin_cdr},
    {"cons", cact_builtin_cons},
    {"null?", cact_builtin_is_nil},
    {"pair?", cact_builtin_is_pair},

    {"boolean?", cact_builtin_is_boolean},
    {"not", cact_builtin_not},

    {"eq?", cact_builtin_eq},
    {"eqv?", cact_builtin_eqv},
    {"equal?", cact_builtin_equal},

    {"display", cact_builtin_display},
    {"newline", cact_builtin_newline},

    {"load", cact_builtin_load},
    {"exit", cact_builtin_exit},

    {"collect-garbage", cact_builtin_collect_garbage},
    {"interaction-environment", cact_builtin_interaction_environment},
    {"bound?", cact_builtin_is_bound},

    {"error", cact_builtin_error},
    {"with-exception-handler", cact_builtin_with_exception_handler},

    {"number?", cact_builtin_is_number},
    {"+", cact_builtin_plus},
    {"-", cact_builtin_minus},
    {"*", cact_builtin_times},
    {"/", cact_builtin_divide},
};

#endif // CONFIG_H

