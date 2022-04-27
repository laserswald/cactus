#include "write.h"
#include "storage/obj.h"
#include "pair.h"
#include "err.h"
#include "str.h"
#include "sym.h"

/* Display a list to the given output file. */
int
cact_fdisplay_list(FILE *f, cact_value_t x)
{
    cact_value_t p = x;
    int chars = 0;

    chars += fprintf(f, "(");

    do {
        if (cact_is_pair(p)) {
            chars += cact_fdisplay(f, cact_to_pair(p, "cact_fdisplay_list")->car);
            p = cact_to_pair(p, "cact_fdisplay_list")->cdr;
            chars += fprintf(f, " ");
        } else {
            chars += cact_fdisplay(f, p);
            chars += fprintf(f, ". ");
            break;
        }
    } while (! cact_is_null(p));

    chars += fprintf(f, ")");

    return chars;
}

int
print_list(cact_value_t x)
{
    return cact_fdisplay_list(stdout, x);
}

int
fprint_obj(FILE *f, cact_object_t *obj)
{
    int chars = 0;

    switch (obj->type) {

    case CACT_OBJ_STRING:
        return fprintf(f, "%s", ((cact_string_t*)obj)->str);

    case CACT_OBJ_PAIR:
        return cact_fdisplay_list(f, CACT_OBJ_VAL(obj));

    case CACT_OBJ_PROCEDURE:
        return fprintf(f, "#<procedure: %p>", (void*)obj);

    case CACT_OBJ_ENVIRONMENT:
        return fprintf(f, "#<environment: %p>", (void*)obj);

    case CACT_OBJ_CONT:
        return fprintf(f, "#<continuation: %p>", (void*)obj);

    case CACT_OBJ_VECTOR:
        return fprintf(f, "#<vector: %p>", (void*)obj);

    case CACT_OBJ_ERROR: {
        cact_error_t *err = (cact_error_t *)obj;
        chars += fprintf(f, "; error '%s' : ", err->msg);
        chars += cact_fdisplay(f, err->ctx);
        break;
    }

    }

    return chars;
}

int
cact_fdisplay(FILE *f, cact_value_t x)
{
    switch (x.type) {
    case CACT_TYPE_UNDEF:
        return 0;
    case CACT_TYPE_NULL:
        return fprintf(f, "()");
    case CACT_TYPE_FIXNUM:
        return fprintf(f, "%li", x.as.fixnum);
    case CACT_TYPE_FLONUM:
        return fprintf(f, "%f", x.as.flonum);
    case CACT_TYPE_BOOL:
        return fprintf(f, "%s", x.as.boolean ? "#t" : "#f");
    case CACT_TYPE_CHAR:
        return fprintf(f, "%c", x.as.character);
    case CACT_TYPE_SYM:
        return fprintf(f, "%s", x.as.symbol->sym);
    case CACT_TYPE_OBJ:
        return fprint_obj(f, x.as.object);
    }
    return 0;
}

int
cact_display(cact_value_t x)
{
    return cact_fdisplay(stdout, x);
}
