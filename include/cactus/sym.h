#ifndef sym_h_INCLUDED
#define sym_h_INCLUDED

#include "cactus/val.h"
#include "cactus/internal/tree.h"

struct cactus;

struct cact_symbol {
	char* sym;
	RB_ENTRY(cact_symbol) entry;
};

RB_HEAD(cact_symbol_table, cact_symbol);
void cact_symbol_table_init(struct cact_symbol_table *);

struct cact_symbol *cact_get_symbol(struct cactus *, const char *);
struct cact_val cact_make_symbol(struct cactus *, const char *);
int cact_symbol_cmp(const void *l, const void *r);

DEFINE_VALUE_CHECK(cact_is_symbol, CACT_TYPE_SYM)
DEFINE_VALUE_CONV(CACT_TYPE_SYM, struct cact_symbol*, cact_to_symbol, symbol)

#endif // sym_h_INCLUDED

