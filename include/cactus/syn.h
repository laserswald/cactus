#ifndef __CACTUS_SYN_H__
#define __CACTUS_SYN_H__

/*
 * Syntax recognizers.
 */

#include <stdbool.h>
#include "cactus/core.h"
#include "cactus/val.h"

bool is_self_evaluating(struct cact_val x);
bool is_variable(struct cactus *cact, struct cact_val x);
bool is_quotation(struct cactus *cact, struct cact_val x);
bool is_conditional(struct cactus *cact, struct cact_val x);
bool is_definition(struct cactus *cact, struct cact_val x);
bool is_lambda(struct cactus *cact, struct cact_val x);
bool is_sequence(struct cactus *cact, struct cact_val x);
bool is_assignment(struct cactus *cact, struct cact_val x);
bool is_application(struct cactus *cact, struct cact_val x);
bool is_tagged_pair(struct cactus *cact, const char* tag, struct cact_val x);

#endif /* __CACTUS_SYN_H__ */
