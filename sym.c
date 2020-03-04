#include "xmalloc.h"
#include "sexp.h"
#include "sym.h"
#include "core.h"
#include "table.h"

TABLE_GENERATE(cact_symbol_table, char *, struct cact_symbol *)

struct cact_val 
cact_get_symbol(struct cactus *cact, char *symname)
{
	struct cact_symbol *sym;
	sym = *TABLE_FIND(cact_symbol_table, &cact->interned_syms, symname);
	if (! sym) {
		// insert it
		sym = xcalloc(1, sizeof(struct cact_symbol));
		sym->sym = symname;

		TABLE_ENTER(cact_symbol_table, &cact->interned_syms, symname, sym);
	}

	struct cact_val v;
	v.type = CACT_TYPE_OBJ;
	v.as.object = cact_store_allocate(&cact->store);
	v.as.object->type = CACT_OBJ_SYMBOL;
	v.as.object->as.sym = sym;
	return v;
}

