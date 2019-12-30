#ifndef sym_h_INCLUDED
#define sym_h_INCLUDED

#include "sexp.h"

cact_val *cact_make_symbol(char *symname);
int symcmp(cact_symbol *a, cact_symbol *b);

#endif // sym_h_INCLUDED

