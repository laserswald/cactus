#ifndef sym_h_INCLUDED
#define sym_h_INCLUDED

#include "table.h"

typedef struct cact_symbol {
    char* sym;
} cact_symbol;

TABLE_DECL(cact_symbol_table, struct cact_symbol)
int symcmp(cact_symbol *a, cact_symbol *b);
struct cact_val *cact_make_symbol(char *);

#include "core.h"
struct cactus;
struct cact_val *cact_get_symbol(struct cactus *, char *);

#endif // sym_h_INCLUDED

