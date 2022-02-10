#ifndef str_h_INCLUDED
#define str_h_INCLUDED

#include "core.h"
#include "storage/obj.h"

struct cact_string {
	struct cact_obj obj;
	char *str;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_STRING,      struct cact_string *, cact_to_string,    str)
DEFINE_OBJECT_CHECK(cact_is_string, CACT_OBJ_STRING)

struct cact_val cact_make_string(struct cactus *, const char *);

void cact_mark_string(struct cact_obj *);
void cact_destroy_string(struct cact_obj *);

#endif // str_h_INCLUDED

