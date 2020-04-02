#ifndef write_h_INCLUDED
#define write_h_INCLUDED

#include <stdio.h>

#include "cactus/val.h"

int fprint_obj(FILE *f, struct cact_obj *);
int fprint_list(FILE *f, struct cact_val x);
int fprint_sexp(FILE *f, struct cact_val x);
int print_list(struct cact_val x);
int print_sexp(struct cact_val x);

#endif // write_h_INCLUDED

