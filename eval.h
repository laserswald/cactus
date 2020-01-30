#ifndef EVAL_H
#define EVAL_H

#include "core.h"

struct cact_val *special_define(struct cactus *, struct cact_val *);
struct cact_val *special_if(struct cactus *, struct cact_val *);
struct cact_val *special_begin(struct cactus *, struct cact_val *);
struct cact_val *special_set_bang(struct cactus *, struct cact_val *);
struct cact_val *special_lambda(struct cactus *, struct cact_val *);
struct cact_val *special_quote(struct cactus *, struct cact_val *);

struct cact_val *cact_eval(struct cactus *, struct cact_val *);

#endif // cact_eval_h_INCLUDED

