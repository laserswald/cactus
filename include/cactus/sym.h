#ifndef sym_h_INCLUDED
#define sym_h_INCLUDED

#include "cactus/val.h"
#include "cactus/internal/table.h"

struct cactus;

typedef struct cact_symbol {
    char* sym;
} cact_symbol;

STRING_TABLE_DECL(cact_symbol_table, struct cact_symbol)
void cact_symbol_table_init(struct cact_symbol_table *);

int symcmp(cact_symbol *a, cact_symbol *b);
struct cact_val cact_get_symbol(struct cactus *, const char *);

DEFINE_VALUE_CHECK(cact_is_symbol, CACT_TYPE_SYM)
DEFINE_VALUE_CONV(CACT_TYPE_SYM, struct cact_symbol*, cact_to_symbol, symbol)

#endif // sym_h_INCLUDED

