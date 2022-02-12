#ifndef syn_h_INCLUDED
#define syn_h_INCLUDED

#include <stdbool.h>

#include "core.h"
#include "val.h"

bool is_self_evaluating(struct cact_val x);
bool is_variable(struct cactus *cact, struct cact_val x);
bool is_quotation(struct cactus *cact, struct cact_val x);
bool is_assignment(struct cactus *cact, struct cact_val x);
bool is_definition(struct cactus *cact, struct cact_val x);
bool is_conditional(struct cactus *cact, struct cact_val x);
bool is_lambda(struct cactus *cact, struct cact_val x);
bool is_sequence(struct cactus *cact, struct cact_val x);
bool is_application(struct cactus *cact, struct cact_val x);
bool is_inclusion(struct cactus *cact, struct cact_val x);

#endif // syn_h_INCLUDED

