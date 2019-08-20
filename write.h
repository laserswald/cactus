#ifndef write_h_INCLUDED
#define write_h_INCLUDED

#include <stdio.h>
#include "sexp.h"

void fprint_list(FILE *f, Sexp *x);
void fprint_sexp(FILE *f, Sexp *x);
void print_list(Sexp *x);
void print_sexp(Sexp *x);

#endif // write_h_INCLUDED

