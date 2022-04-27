#ifndef str_h_INCLUDED
#define str_h_INCLUDED

#include "core.h"
#include "storage/obj.h"

typedef struct cact_string {
	cact_object_t obj;
	char *str;
} cact_string_t;

DEFINE_OBJECT_CONVERSION(CACT_OBJ_STRING, cact_string_t *, cact_to_string, str)
DEFINE_OBJECT_CHECK(cact_is_string, CACT_OBJ_STRING)

cact_value_t cact_make_string(cact_context_t *, const char *);

void cact_mark_string(cact_object_t *);
void cact_destroy_string(cact_object_t *);

#endif // str_h_INCLUDED

