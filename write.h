#ifndef write_h_INCLUDED
#define write_h_INCLUDED

#include <stdio.h>
#include "sexp.h"

int fprint_list(FILE *f, cact_val x);
int fprint_sexp(FILE *f, cact_val x);
int print_list(cact_val x);
int print_sexp(cact_val x);

#endif // write_h_INCLUDED

