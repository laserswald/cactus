#include "mac.h"

cact_value_t
cact_make_unhygenic_macro(cact_context_t *cact, cact_procedure_t *transformer)
{
	cact_macro_t *mac = (cact_macro_t *) cact_alloc(cact, CACT_OBJ_MACRO);
	mac->type = CACT_MACRO_UNHYGENIC;
	mac->as.unhygenic = transformer;
	return CACT_OBJ_VAL(mac);
}

cact_value_t
cact_expand_macro(cact_context_t *cact, cact_macro_t *mac, cact_value_t expr)
{
	cact_value_t transformed = CACT_UNDEF_VAL; 

	switch (mac->type) {
		case CACT_MACRO_UNHYGENIC: {
			transformed = cact_proc_apply(cact, mac->as.unhygenic, cact_cdr(cact, expr));
		}
		default: {
			abort();
		}
	}

	return transformed;
}

/*
 * Expand all the macros in this expression level.
 */
cact_value_t
cact_expand_level(cact_context_t *cact, cact_value_t expr)
{
	// We need to expand macros as many times as we need until we are done.
	bool expanded = false;
	do {
		// We only expand macros that look like procedure applications.
	    if (! cact_is_pair(cact, expr)) {
		    break;
	    }

	    // Macros must be bound to an identifier first, so bail if we don't have that.
	    cact_value_t operator_name = cact_car(cact, expr); 
	    if (! cact_is_symbol(cact, operator_name)) {
		    break;
	    }

	    cact_frame_t *cc = cact_current_cont(cact);
	    cact_value_t operator = cact_env_lookup(cact, cc->env, operator_name);    
	    if (! cact_is_macro(cact, operator)) {
		    break;
	    }

	    cact_macro_t *mac = cact_to_macro(operator, "<macro-expansion>");
	    expr = cact_expand_macro(cact, mac, expr);
	    expanded = true;

    } while (expanded);

    return expr;
}

/*
 * Expand all the macros in this expression level.
 */
cact_value_t
cact_expand(cact_context_t *cact, cact_value_t expr)
{
	// We only expand macros that look like procedure applications.
    if (! cact_is_pair(cact, expr)) {
	    break;
    }

    // Expand this level
    expr = cact_expand_level(cact, expr);

    // Expand child levels.
    CACT_LIST_FOR_EACH_PAIR(cact, subpair, expr) {
	    cact_value_t subexpr = cact_car(cact, subpair);
	    cact_value_t expanded_subexpr = cact_expand_level(cact, subexpr);
	    cact_set_car(cact, subpair, expanded_subexpr);
    }

	return expr;
}

