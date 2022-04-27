#include <string.h>

#include "sym.h"

#include "core.h"
#include "val.h"

#include "internal/xmalloc.h"
#include "internal/tree.h"

int
cact_symbol_cmp(const void *l, const void *r)
{
    const cact_symbol_t *lsym, *rsym;
    lsym = l;
    rsym = r;
    return strcmp(lsym->sym, rsym->sym);
}

RB_GENERATE(cact_symbol_table, cact_symbol, entry, cact_symbol_cmp)

void
cact_symbol_table_init(cact_symbol_table_t *symtab)
{
    RB_INIT(symtab);
}

cact_value_t
cact_make_symbol(cact_context_t *cact, const char *symname)
{
    return CACT_SYM_VAL(cact_get_symbol(cact, symname));
}

cact_symbol_t *
cact_get_symbol(cact_context_t *cact, const char *symname)
{
    cact_symbol_t sym = { .sym = symname };

    if (! RB_FIND(cact_symbol_table, &cact->interned_syms, &sym)) {
        RB_INSERT(cact_symbol_table, &cact->interned_syms,
                  xmemdup(&sym, sizeof(cact_symbol_t)));
    }

    return RB_FIND(cact_symbol_table, &cact->interned_syms, &sym);
}

