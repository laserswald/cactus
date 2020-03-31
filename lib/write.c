#include "cactus/write.h"
#include "cactus/obj.h"
#include "cactus/pair.h"
#include "cactus/err.h"
#include "cactus/str.h"
#include "cactus/sym.h"

int
fprint_list(FILE *f, struct cact_val x)
{
    int chars = 0;
    chars += fprintf(f, "(");
    struct cact_val p = x;
    do
    {
        if (cact_is_pair(p))
        {
            chars += fprint_sexp(f, cact_to_pair(p, "fprint_list")->car);
            p = cact_to_pair(p, "fprint_list")->cdr;
            chars += fprintf(f, " ");
        }
        else
        {
            chars += fprint_sexp(f, p);
            chars += fprintf(f, ". ");
            break;
        }
    }
    while (! cact_is_null(p));
    chars += fprintf(f, ")");
    return chars;
}

int
print_list(struct cact_val x)
{
    return fprint_list(stdout, x);
}

static int
fprint_obj(FILE *f, struct cact_val x)
{
    struct cact_obj *obj = x.as.object;
    int chars = 0;

    switch (obj->type)
    {
    case CACT_OBJ_STRING:
        return fprintf(f, "%s", ((struct cact_string*)obj)->str);
    case CACT_OBJ_PAIR:
        return fprint_list(f, x);
    case CACT_OBJ_PROCEDURE:
        return fprintf(f, "#<procedure: %p>", (void*)obj);
    case CACT_OBJ_ENVIRONMENT:
        return fprintf(f, "#<environment: %p>", (void*)obj);
    case CACT_OBJ_ERROR:
    {
        struct cact_error *err = (struct cact_error *)obj;
        chars += fprintf(f, "; error '%s' : ", err->msg);
        chars += fprint_sexp(f, err->ctx);
        break;
    }
    }
    return chars;
}

int
fprint_sexp(FILE *f, struct cact_val x)
{
    switch (x.type)
    {
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
        return fprint_obj(f, x);
    }
    return 0;
}

int
print_sexp(struct cact_val x)
{
    return fprint_sexp(stdout, x);
}
