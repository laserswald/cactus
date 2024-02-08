
cact_value_t
cact_expand_unhygenic(
	cact_context_t *cact, 
	cact_procedure_t *transformer, 
	cact_value_t expression
) {
}

// Represents one arm of a `syntax-rules` expression.
typedef struct cact_syntax_rule {
	cact_value_t pattern;  // The pattern to match
	cact_value_t template; // The template to fill by the matched bindings
} cact_syntax_rule_t;

// Valid return values for cact_pattern_matches()
typedef enum cact_pattern_match_status {
	CACT_INVALID_PATTERN = -1,
	CACT_PATTERN_DID_NOT_MATCH = 0,
	CACT_PATTERN_MATCHED = 1,
} cact_pattern_match_status_t;

/*
 * Attempt to match a pattern using the expression, the given bindings, and the ellipsis
 * symbol.
 * 
 * If the match succeeded, then CACT_PATTERN_MATCHED is returned, and the bindings passed
 * to `bindings` are updated with the correct bindings.
 * 
 * If the match failed, CACT_PATTERN_DID_NOT_MATCH is returned and the bindings are left 
 * alone.
 * 
 * If a pattern is invalid, CACT_INVALID_PATTERN is returned and the bindings are left as they are.
 */
cact_pattern_match_status_t
cact_pattern_matches(
	const cact_context_t *cact, 
	const cact_value_t pattern, 
	const cact_value_t expression, 
	cact_env_t *bindings,
	const cact_symbol_t *ellipsis
) {
	struct cact_env *original_bindings = bindings;

	// The following cases are basically directly lifted from R7RS, section 4.3.2. 
	// 
	// If this looks a lot like an interpreter, that's because it kind of is.

	if (is_constant(pattern)) {
		if (cact_is_equal(cact, pattern, expression)) {
			return CACT_PATTERN_MATCHED;
		}
		return CACT_PATTERN_DID_NOT_MATCH;
	}

	if (pattern_is_underscore(pattern)) {
		// We don't bind anything.
		return CACT_PATTERN_MATCHED;
	}

	if (is_identifier(pattern)) {
		// Bind the whole expression to the identifier, and return true.
		cact_env_define(cact, bindings, pattern, expression);
		return CACT_PATTERN_MATCHED;
	}

	if (cact_is_pair(pattern)) {
		// Ensure that the pattern and the expression are both lists.
		if (! cact_is_pair(expression)) {
			goto failed_to_match;
		}

		struct cact_val expression_walker = expression;

		// For each element in the pattern, match with the corresponding element in the list.
		CACT_LIST_FOR_EACH_ELEM(cact, subpattern, pattern) {
			struct cact_val subexpression = cact_car(cact, expression_walker);
			int result = cact_pattern_matches(cact, subpattern, subexpression, bindings, ellipsis);
			switch (result) {
				case CACT_PATTERN_DID_NOT_MATCH:
					goto CACT_PATTERN_DID_NOT_MATCH;
				case CACT_INVALID_PATTERN:
					return CACT_INVALID_PATTERN;
				case CACT_PATTERN_MATCHED:
					break;
			}
			// default case: keep going
			expression_walker = cact_cdr(expression_walker);
		}

		return CACT_PATTERN_MATCHED;
	}

	if (cact_is_vec(pattern)) {
		// Ensure that the pattern and the expression are both vectors.
		if (! cact_is_vec(expression)) {
			goto failed_to_match;
		}
	}

	// No other data types are valid patterns.
	return CACT_INVALID_PATTERN;

failed_to_match:
	bindings = original_bindings;
	return CACT_PATTERN_DID_NOT_MATCH;
}

bool
cact_apply_template(cact_context_t *cact, cact_syntax_rule_t *rule, 
	                cact_value_t expression, cact_env_t *bindings) 
{
	
}

// Represents a syntax-rules transformer.
struct cact_syntax_rules {
	// The ellipsis identifier.
	struct cact_symbol *ellipsis;      

	// Identifiers that should not be expanded (just for extra keywords and stuff like that)
	struct cact_symbol_arr literals;   

	// Each valid combination of pattern and template.
	struct cact_syntax_rule_arr rules;
} cact_syntax_rules_t;

struct cact_val
cact_expand_syntax_rules(struct cactus *cact, struct cact_syntax_rules *rules, struct cact_val expression)
{	
	ARRAY_FOREACH() {
		cact_pattern_match_status_t status = 
			cact_pattern_matches(
				cact, 
				pattern, 
				expression, 
				bindings, 
				ellipsis
			);

		if (status < 0) {
			// error
		} else if (status == 0) {
			continue;
		} else {

		}
	}
}

enum {
	CACT_MACRO_UNHYGENIC,
	CACT_MACRO_SYNTAX_RULES,
} cact_macro_type_t;

struct cact_macro {
	struct cact_obj obj;
	enum cact_macro_type type;
	union {		
		struct cact_proc         *unhygenic;
		struct cact_syntax_rules syntax_rules;
	} as;
} cact_macro_t;

struct cact_val
cact_make_unhygenic_macro(struct cactus *cact, struct cact_proc *transformer)
{
	cact_macro_t *mac = (cact_macro_t *)cact_alloc(cact, CACT_OBJ_MACRO);
	mac->type = CACT_MACRO_UNHYGENIC;
	mac->as.unhygenic = transformer;
	return CACT_OBJ_VAL(mac);
}

struct cact_val
cact_make_syntax_rules_macro(struct cactus *cact, )
{
	cact_macro_t *mac = (cact_macro_t *)cact_alloc(cact, CACT_OBJ_MACRO);
	mac->type = CACT_MACRO_SYNTAX_RULES;
	mac->as.syntax_rules = transformer;
	return CACT_OBJ_VAL(mac);
}

/*
 * Expand a single macro in an expression.
 */
struct cact_val
cact_expand_macro(struct cactus *cact, cact_macro_t *macro, struct cact_val expression)
{

}

/*
 * Expand all the macros in the expression.
 */
struct cact_val
cact_expand_macros(struct cactus *cact, struct cact_val expression)
{
	if (! cact_is_pair(expression)) {
		return expression;
	}

	bool applied_macro = false;
	do {
		// look up the name
		// 
	} while ();
}
