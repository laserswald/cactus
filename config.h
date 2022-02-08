#ifndef CONFIG_H
#define CONFIG_H

#include "cactus/eval.h"
#include "cactus/builtin.h"

struct cact_builtin builtins[] = {
    // Equality
    {"eq?", cact_builtin_eq},
    {"eqv?", cact_builtin_eqv},
    {"equal?", cact_builtin_equal},

    // Booleans
    {"boolean?", cact_builtin_is_boolean},
    {"not", cact_builtin_not},

    // Numbers
    {"number?", cact_builtin_is_number},
    {"+", cact_builtin_plus},
    {"-", cact_builtin_minus},
    {"*", cact_builtin_times},
    {"/", cact_builtin_divide},

    // Pairs
    {"car", cact_builtin_car},
    {"cdr", cact_builtin_cdr},
    {"cons", cact_builtin_cons},
    {"null?", cact_builtin_is_nil},
    {"pair?", cact_builtin_is_pair},

    // Vectors
    {"vector?", cact_builtin_is_vector},
    {"make-vector", cact_builtin_make_vector},
    {"vector-ref", cact_builtin_vector_ref},
    {"vector-set!", cact_builtin_vector_set},
    {"vector-length", cact_builtin_vector_length},

    {"procedure?", cact_builtin_is_procedure},

    {"display", cact_builtin_display},
    {"newline", cact_builtin_newline},

    {"error", cact_builtin_error},

    /*
    {"raise", cact_builtin_raise},
    {"error-object?", cact_builtin_is_error_object},
    {"error-message", cact_builtin_error_message},
    {"error-irritants", cact_builtin_error_irritants},
    */

    {"load", cact_builtin_load},
    {"exit", cact_builtin_exit},
    {"interaction-environment", cact_builtin_interaction_environment},
    {"collect-garbage", cact_builtin_collect_garbage},
    {"bound?", cact_builtin_is_bound},
};

#endif // CONFIG_H

