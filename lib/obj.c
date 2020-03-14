
#include "cactus/obj.h"

const char* cact_obj_show_type(enum cact_obj_type t)
{
    switch (t) {
		case CACT_OBJ_PAIR: return "pair";
		case CACT_OBJ_STRING: return "string";
		case CACT_OBJ_PROCEDURE: return "procedure";
		case CACT_OBJ_ENVIRONMENT: return "environment";
		case CACT_OBJ_ERROR: return "error";
    }
    return NULL;
}
