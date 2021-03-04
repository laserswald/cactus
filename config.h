#ifndef CONFIG_H
#define CONFIG_H

#include "cactus/eval.h"
#include "cactus/builtin.h"

struct cact_builtin builtins[] = {
	// The special forms.

    {"car", cact_builtin_car, 1},
    {"cdr", cact_builtin_cdr, 1},
    {"cons", cact_builtin_cons, 2},
    {"null?", cact_builtin_is_nil, 1},
    {"pair?", cact_builtin_is_pair, 1},

    {"boolean?", cact_builtin_is_boolean, 1},
    {"not", cact_builtin_not, 1},

    {"eq?", cact_builtin_eq, 2},
    {"eqv?", cact_builtin_eqv, 2},
    {"equal?", cact_builtin_equal, 2},

    {"display", cact_builtin_display, 1},
    {"newline", cact_builtin_newline, 0},

    {"load", cact_builtin_load, 1},
    {"exit", cact_builtin_exit, 0},

    {"collect-garbage", cact_builtin_collect_garbage, 0},
    {"interaction-environment", cact_builtin_interaction_environment, 0},
    {"bound?", cact_builtin_is_bound, 1},

    {"error", cact_builtin_error, 1},
    {"with-exception-handler", cact_builtin_with_exception_handler, 2},
    {"raise", cact_builtin_raise, 1},

    {"number?", cact_builtin_is_number, 1},
    {"+", cact_builtin_plus, 2},
    {"-", cact_builtin_minus, 2},
    {"*", cact_builtin_times, 2},
    {"/", cact_builtin_divide, 2},
};

#endif // CONFIG_H

