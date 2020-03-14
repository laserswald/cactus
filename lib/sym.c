#include "cactus/sym.h"

#include "cactus/core.h"
#include "cactus/val.h"

#include "cactus/internal/xmalloc.h"
#include "cactus/internal/table.h"

STRING_TABLE_GENERATE(cact_symbol_table, struct cact_symbol)

void
cact_symbol_table_init(struct cact_symbol_table *symtab)
{
	STRING_TABLE_INIT(symtab);
}

struct cact_val 
cact_get_symbol(struct cactus *cact, const char *symname)
{
	if (! TABLE_HAS(cact_symbol_table, &cact->interned_syms, symname)) {
		struct cact_symbol sym;
		sym.sym = symname;
		TABLE_ENTER(cact_symbol_table, &cact->interned_syms, symname, sym);
	}

	return CACT_SYM_VAL(TABLE_FIND(cact_symbol_table, &cact->interned_syms, symname));
}

