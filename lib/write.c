#include "write.h"
#include "obj.h"

int
fprint_list(FILE *f, cact_val x)
{
	int chars = 0;
    chars += fprintf(f, "(");
    cact_val p = x;
    while (! cact_is_null(p)) {
        if (cact_is_pair(p)) {
            chars += fprint_sexp(f, cact_car(p));
            chars += fprintf(f, " ");
            p = cact_cdr(p);
        } else {
            chars += fprintf(f, ". ");
            chars += fprint_sexp(f, p);
            break;
        }
    }
    chars += fprintf(f, ")");
    return chars;
}

int
print_list(cact_val x) {
    return fprint_list(stdout, x);
}

static int
fprint_obj(FILE *f, struct cact_val x)
{
	struct cact_obj *obj = x.as.object;
	int chars = 0;

	switch (obj->type) {
    case CACT_OBJ_STRING: return fprintf(f, "%s", obj->as.str.str);
    case CACT_OBJ_SYMBOL: return fprintf(f, "%s", obj->as.sym.sym);
    case CACT_OBJ_PAIR:  
	    return fprint_list(f, x);
    case CACT_OBJ_PROCEDURE: return fprintf(f, "#<procedure: %p>", (void*)obj);
    case CACT_OBJ_ENVIRONMENT: return fprintf(f, "#<environment: %p>", (void*)obj);
    case CACT_OBJ_ERROR:
        chars += fprintf(f, "; error '%s' : ", obj->as.err.msg);
        chars += fprint_sexp(f, obj->as.err.ctx);
        break;
	}
    return chars;
}

int
fprint_sexp(FILE *f, cact_val x)
{
    switch (x.type) {
	case CACT_TYPE_UNDEF:  return 0;
    case CACT_TYPE_NULL:   return fprintf(f, "()");
    case CACT_TYPE_FIXNUM: return fprintf(f, "%li", x.as.fixnum);
    case CACT_TYPE_FLONUM: return fprintf(f, "%f", x.as.flonum);
    case CACT_TYPE_BOOL:   return fprintf(f, "%s", x.as.boolean ? "#t" : "#f");
    case CACT_TYPE_CHAR:   return fprintf(f, "%c", x.as.character);
    case CACT_TYPE_OBJ:    return fprint_obj(f, x);
    }
    return 0;
}

int
print_sexp(cact_val x) {
    return fprint_sexp(stdout, x);
}
