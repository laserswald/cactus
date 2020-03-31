#include <string.h>

#include "cactus/sym.h"

#include "cactus/core.h"
#include "cactus/val.h"

#include "cactus/internal/xmalloc.h"
#include "cactus/internal/tree.h"

int
cact_symbol_cmp(const void *l, const void *r)
{
    const struct cact_symbol *lsym, *rsym;
    lsym = l;
    rsym = r;
    return strcmp(lsym->sym, rsym->sym);
}

RB_GENERATE(cact_symbol_table, cact_symbol, entry, cact_symbol_cmp)

void
cact_symbol_table_init(struct cact_symbol_table *symtab)
{
    RB_INIT(symtab);
}

struct cact_val
cact_make_symbol(struct cactus *cact, const char *symname)
{
    return CACT_SYM_VAL(cact_get_symbol(cact, symname));
}

struct cact_symbol *
cact_get_symbol(struct cactus *cact, const char *symname)
{
    struct cact_symbol sym = { .sym = symname };

    if (! RB_FIND(cact_symbol_table, &cact->interned_syms, &sym))
    {
        RB_INSERT(cact_symbol_table, &cact->interned_syms,
                  xmemdup(&sym, sizeof(struct cact_symbol)));
    }

    return RB_FIND(cact_symbol_table, &cact->interned_syms, &sym);
}

