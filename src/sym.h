#ifndef sym_h_INCLUDED
#define sym_h_INCLUDED

#include "val.h"
#include "internal/tree.h"
#include "internal/array.h"

typedef struct cact_context cact_context_t;

typedef struct cact_symbol {
	const char *sym;
	RB_ENTRY(cact_symbol) entry;
} cact_symbol_t;

ARRAY_DECL(cact_symbol_array, struct cact_symbol);
typedef struct cact_symbol_array cact_symbol_array_t;

RB_HEAD(cact_symbol_table, cact_symbol);
typedef struct cact_symbol_table cact_symbol_table_t;

void 
cact_symbol_table_init(cact_symbol_table_t *);

cact_symbol_t *
cact_get_symbol(cact_context_t *, const char *);

cact_value_t
cact_make_symbol(cact_context_t *, const char *);

int 
cact_symbol_cmp(const void *l, const void *r);

DEFINE_VALUE_CHECK(cact_is_symbol, CACT_TYPE_SYM)
DEFINE_VALUE_CONV(CACT_TYPE_SYM, cact_symbol_t *, cact_to_symbol, symbol)

#endif // sym_h_INCLUDED

