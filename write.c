#include "write.h"

void
fprint_list(FILE *f, Sexp *x)
{
    fprintf(f, "(");
    Sexp *p = x;
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

void print_list(Sexp *x) {
    fprint_list(stdout, x);
}

void
fprint_sexp(FILE *f, Sexp *x)
{
    if (!x) {
        fprintf(f, "()");
        return;
    }

    switch (x->t) {
    case TYPE_INT:
        fprintf(f, "%i", x->i);
        break;
    case TYPE_DOUBLE:
        fprintf(f, "%f", x->f);
        break;
    case TYPE_STRING:
        fprintf(f, "%s", x->s.str);
        break;
    case TYPE_SYMBOL:
        fprintf(f, "#<symbol: '%s'>", x->a.sym);
        break;
    case TYPE_PAIR:
        print_list(x);
        break;
    case TYPE_CLOSURE:
        fprintf(f, "#<closure: %p>", (void*)x->c);
        break;
    case TYPE_ENVIRONMENT:
        fprintf(f, "#<environment: %p>", (void*)x->e);
        break;
    case TYPE_PORT:
        break;
    case TYPE_ERROR:
        fprintf(f, "; error '%s' : ", x->x.msg);
        print_sexp(x->x.ctx);
        break;
    case TYPE_UNDEF:
        fprintf(f, "UNDEFINED");
        break;
    }
}

void print_sexp(Sexp *x) {
    fprint_sexp(stdout, x);
}
