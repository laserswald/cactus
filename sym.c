#include "xmalloc.h"
#include "sexp.h"
#include "sym.h"

TABLE_GENERATE(cact_symbol_table, struct cact_symbol)

struct cact_val *cact_make_symbol(char *name)
{
	struct cact_symbol *sym = xcalloc(1, sizeof(struct cact_symbol));

	struct cact_val *v = malloc(sizeof(struct cact_val));
	v->t = CACT_TYPE_SYMBOL;
	v->a = sym;
	return v;
}

struct cact_val *cact_get_symbol(struct cactus *cact, char *symname)
{
	struct cact_symbol *sym;
	sym = TABLE_FIND(cact_symbol_table, &cact->interned_syms, symname);
	if (! sym) {
		// insert it
		sym = xcalloc(1, sizeof(struct cact_symbol));
		sym->sym = symname;

		TABLE_ENTER(cact_symbol_table, &cact->interned_syms, symname, sym);
	}

	struct cact_val *v = malloc(sizeof(struct cact_val));
	v->t = CACT_TYPE_SYMBOL;
	v->a = sym;
	return v;
}

