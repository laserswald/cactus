#ifndef cact_builtin_h_INCLUDED
#define cact_builtin_h_INCLUDED

#include "cactus/proc.h"

#define BUILTIN_FUNC(name) \
	struct cact_val name(struct cactus *, struct cact_val)

BUILTIN_FUNC(cact_builtin_if);
BUILTIN_FUNC(cact_builtin_car);
BUILTIN_FUNC(cact_builtin_cdr);
BUILTIN_FUNC(cact_builtin_cons);
BUILTIN_FUNC(cact_builtin_is_nil);
BUILTIN_FUNC(cact_builtin_is_pair);
BUILTIN_FUNC(cact_builtin_is_number);
BUILTIN_FUNC(cact_builtin_is_boolean);
BUILTIN_FUNC(cact_builtin_not);
BUILTIN_FUNC(cact_builtin_eq);
BUILTIN_FUNC(cact_builtin_eqv);
BUILTIN_FUNC(cact_builtin_equal);
BUILTIN_FUNC(cact_builtin_display);
BUILTIN_FUNC(cact_builtin_newline);
BUILTIN_FUNC(cact_builtin_begin);
BUILTIN_FUNC(cact_builtin_exit);
BUILTIN_FUNC(cact_builtin_load);
BUILTIN_FUNC(cact_builtin_plus);
BUILTIN_FUNC(cact_builtin_minus);
BUILTIN_FUNC(cact_builtin_times);
BUILTIN_FUNC(cact_builtin_divide);
BUILTIN_FUNC(cact_builtin_load);
BUILTIN_FUNC(cact_builtin_interaction_environment);
BUILTIN_FUNC(cact_builtin_is_bound);
BUILTIN_FUNC(cact_builtin_collect_garbage);

/* Errors. */
BUILTIN_FUNC(cact_builtin_with_exception_handler);
BUILTIN_FUNC(cact_builtin_raise);
BUILTIN_FUNC(cact_builtin_error);
BUILTIN_FUNC(cact_builtin_is_error_object);
BUILTIN_FUNC(cact_builtin_error_message);
BUILTIN_FUNC(cact_builtin_error_irritants);

#undef BUILTIN_FUNC

#endif // cact_builtin_h_INCLUDED

