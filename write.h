#ifndef write_h_INCLUDED
#define write_h_INCLUDED

#include <stdio.h>
#include "sexp.h"

void fprint_list(FILE *f, cact_val *x);
void fprint_sexp(FILE *f, cact_val *x);
void print_list(cact_val *x);
void print_sexp(cact_val *x);

#endif // write_h_INCLUDED

