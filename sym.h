#ifndef sym_h_INCLUDED
#define sym_h_INCLUDED

#include "sexp.h"

Sexp *make_symbol(char *symname);
int symcmp(Symbol *a, Symbol *b);

#endif // sym_h_INCLUDED

