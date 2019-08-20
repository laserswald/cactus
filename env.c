#include <stdlib.h>
#include "sexp.h"
#include "env.h"

void
envinit(Env *e, Env *parent)
{
	e->parent = parent;
}

Sexp *
envlookup(Env *e, Sexp *key)
{
	if (!e) return NULL;

	Sexp *found_kv = assoc(key, e->list);

	if (!found_kv && e->parent) {
		found_kv = envlookup(e->parent, key);
	}

	if (!found_kv) {
		return NULL;
	}

	return found_kv;
}

void
envadd(Env *e, Sexp *key, Sexp *val)
{
	if (!e) 
		return;

	Sexp *found_kv = envlookup(e, key);

	if (found_kv) {
		found_kv->p.cdr = val;
	} else {
		e->list = acons(key, val, e->list);
	}
}
