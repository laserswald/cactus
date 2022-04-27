#ifndef write_h_INCLUDED
#define write_h_INCLUDED

#include <stdio.h>

#include "val.h"

int fprint_obj(FILE *f, cact_object_t *);
int fprint_list(FILE *f, cact_value_t x);
int fprint_sexp(FILE *f, cact_value_t x);
int print_list(cact_value_t x);
int cact_display(cact_value_t x);
int cact_fdisplay(FILE *f, cact_value_t x);

#endif // write_h_INCLUDED

