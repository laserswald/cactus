#include "write.h"

void
fprint_list(FILE *f, cact_val *x)
{
    fprintf(f, "(");
    cact_val *p = x;
    while (p != NULL) {
        if (is_pair(p)) {
            fprint_sexp(f, car(p));
            fprintf(f, " ");
            p = cdr(p);
        } else {
            fprintf(f, ". ");
            fprint_sexp(f, p);
            break;
        }
    }
    fprintf(f, ")");
}

void print_list(cact_val *x) {
    fprint_list(stdout, x);
}

void
fprint_sexp(FILE *f, cact_val *x)
{
    if (!x) {
        fprintf(f, "()");
        return;
    }

    switch (x->t) {
    case CACT_TYPE_BOOLEAN:
        fprintf(f, "%s", x->b ? "#t" : "#f");
        break;
    case CACT_TYPE_INT:
        fprintf(f, "%i", x->i);
        break;
    case CACT_TYPE_DOUBLE:
        fprintf(f, "%f", x->f);
        break;
    case CACT_TYPE_STRING:
        fprintf(f, "%s", x->s.str);
        break;
    case CACT_TYPE_SYMBOL:
        fprintf(f, "#<symbol: '%s'>", x->a.sym);
        break;
    case CACT_TYPE_PAIR:
        print_list(x);
        break;
    case CACT_TYPE_PROCEDURE:
        fprintf(f, "#<procedure: %p>", (void*)x->c);
        break;
    case CACT_TYPE_ENVIRONMENT:
        fprintf(f, "#<environment: %p>", (void*)x->e);
        break;
    case CACT_TYPE_PORT:
        break;
    case CACT_TYPE_ERROR:
        fprintf(f, "; error '%s' : ", x->x.msg);
        fprint_sexp(f, x->x.ctx);
        break;
    case CACT_TYPE_UNDEF:
        fprintf(f, "UNDEFINED");
        break;
    }
}

void print_sexp(cact_val *x) {
    fprint_sexp(stdout, x);
}
