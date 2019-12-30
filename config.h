#ifndef CONFIG_H
#define CONFIG_H

#include "builtin.h"

BuiltinTable builtins[] = {
    {"car", cact_builtin_car},
    {"cdr", cact_builtin_cdr},
    {"cons", cact_builtin_cons},
    {"null?", cact_builtin_is_nil},
    {"pair?", cact_builtin_is_pair},

    {"boolean?", cact_builtin_is_boolean},
    {"not", cact_builtin_not},

    {"eq?", cact_builtin_eq},
    {"display", cact_builtin_display},
    {"newline", cact_builtin_newline},
    {"begin", cact_builtin_begin},
    {"exit", cact_builtin_exit},
    {"load", cact_builtin_load},

    {"number?", cact_builtin_is_number},
    {"+", cact_builtin_plus},
    {"-", cact_builtin_minus},
    {"*", cact_builtin_times},
    {"/", cact_builtin_divide},
};

#endif // CONFIG_H

