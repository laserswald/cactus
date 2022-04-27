#ifndef syn_h_INCLUDED
#define syn_h_INCLUDED

#include <stdbool.h>

#include "core.h"
#include "val.h"

bool is_self_evaluating(cact_value_t x);
bool is_variable(cact_context_t *cact, cact_value_t x);
bool is_quotation(cact_context_t *cact, cact_value_t x);
bool is_assignment(cact_context_t *cact, cact_value_t x);
bool is_definition(cact_context_t *cact, cact_value_t x);
bool is_conditional(cact_context_t *cact, cact_value_t x);
bool is_lambda(cact_context_t *cact, cact_value_t x);
bool is_sequence(cact_context_t *cact, cact_value_t x);
bool is_application(cact_context_t *cact, cact_value_t x);

#endif // syn_h_INCLUDED

