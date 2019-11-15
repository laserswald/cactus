#ifndef CONFIG_H
#define CONFIG_H

#include "builtin.h"

BuiltinTable builtins[] = {
    {"car", builtin_car},
    {"cdr", builtin_cdr},
    {"cons", builtin_cons},
    {"null?", builtin_is_nil},
    {"pair?", builtin_is_pair},

    {"boolean?", builtin_is_boolean},
    {"not", builtin_not},

    {"eq?", builtin_eq},
    {"display", builtin_display},
    {"newline", builtin_newline},
    {"begin", builtin_progn},
    {"exit", builtin_exit},
    {"load", builtin_load},

    {"number?", builtin_is_number},
    {"+", builtin_plus},
    {"-", builtin_minus},
    {"*", builtin_times},
    {"/", builtin_divide},
};

#endif // CONFIG_H

