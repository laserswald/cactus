#ifndef CACT_VAL_H
#define CACT_VAL_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * The possible types that a value can take on. 
 * 
 * See obj.h for the possible types for objects.
 */
enum cact_type {
	CACT_TYPE_UNDEF,
	CACT_TYPE_NULL,
	CACT_TYPE_FIXNUM,
	CACT_TYPE_FLONUM,
	CACT_TYPE_BOOL,
	CACT_TYPE_CHAR,
	CACT_TYPE_SYM,
	CACT_TYPE_OBJ,
};

/*
 * The core value type for Cactus.
 * 
 * Vals store what type they are and any associated data that the value has. For 
 * the undefined value and the empty list, the union does not contain any data and 
 * may have any bit pattern, but usually it will be zeroed out.
 * 
 * Symbols are references to a table defined elsewhere, and objects are managed
 * by the garbage collector (see store.{h,c} for details).
 */
struct cact_val {
	enum cact_type type;
	union {
		long      fixnum;
		double    flonum;
		bool      boolean;
		char      character;
		struct cact_symbol *symbol;
		struct cact_obj *object;
	} as;
};

/*
 * Helper macros that make encapsulating raw C values into Cactus values easier.
 */
#define CACT_NULL_VAL    ((struct cact_val){.type=CACT_TYPE_NULL})
#define CACT_UNDEF_VAL   ((struct cact_val){.type=CACT_TYPE_UNDEF})
#define CACT_FIX_VAL(n)  ((struct cact_val){.type=CACT_TYPE_FIXNUM, .as.fixnum = (n)})
#define CACT_FLO_VAL(n)  ((struct cact_val){.type=CACT_TYPE_FLONUM, .as.flonum = (n)})
#define CACT_BOOL_VAL(p) ((struct cact_val){.type=CACT_TYPE_BOOL, .as.boolean = (p)})
#define CACT_CHAR_VAL(p) ((struct cact_val){.type=CACT_TYPE_CHAR, .as.character = (p)})
#define CACT_SYM_VAL(p)  ((struct cact_val){.type=CACT_TYPE_SYM, .as.symbol = (p)})
#define CACT_OBJ_VAL(p)  ((struct cact_val){.type=CACT_TYPE_OBJ, .as.object = (struct cact_obj *)(p)})

/* Get a human-readable string showing what type this value is. */
const char *cact_type_str(struct cact_val x);

/* Get the human-readable string for the value type given. */
const char *cact_val_show_type(enum cact_type t);

/*
 * Define some helper macros that automatically create conversion and checking for a type.
 * 
 * These are often used in the other files that deal with individual value types, but 
 * they are also used below for the value types that have no extra data involved with them.
 */
#define DEFINE_VALUE_CONV(typemarker, returntype, funcname, membername) \
static inline returntype \
funcname(struct cact_val x, char *extras) \
{ \
	if (x.type != typemarker) { \
		fprintf(stderr, "%s: Expected %s, but got %s.\n", extras, cact_val_show_type(typemarker), cact_type_str(x)); \
		abort(); \
	} \
	return x.as.membername; \
}

#define DEFINE_VALUE_CHECK(funcname, typemarker) \
static inline bool \
funcname(struct cact_val x) { \
	return x.type == typemarker; \
}

DEFINE_VALUE_CONV(CACT_TYPE_OBJ, struct cact_obj*, cact_to_obj,   object)

bool cact_is_null(struct cact_val);
DEFINE_VALUE_CHECK(cact_is_undef, CACT_TYPE_UNDEF)
DEFINE_VALUE_CHECK(cact_is_obj,   CACT_TYPE_OBJ)

/*
 * The equality comparison operations.
 */

/* 
 * Compare two values for identity.
 * 
 * As per the standard, `eq?` is the strictest equality operation, and should only be used when
 * the _exact_ same values are expected. For most values, this does not work because of unpacking
 * and re-packing the data into new values. However, in Cactus, it does work for the following:
 * 
 * - Symbols, as defined in the standard
 * - Objects, if the two object values point to the same spot in the garbage collector
 * - The empty list, aka null, aka '()
 * - Booleans 
 * 
 * NOTE: Comparing any value with the undefined value, INCLUDING the undefined value, results
 * in FALSE.
 */
bool cact_val_eq(struct cact_val l, struct cact_val r);

/* 
 * Compare two values for equivalence.
 * 
 * `eqv` allows for a moderate amount of introspection into the contents of the given value
 * in order to provide a more immediately useful version of equality semantics. In addition
 * to returning TRUE for everything that would return true for `eq?`, `eqv?` correctly compares
 * these types of values:
 * 
 * - Fixnums
 * - Flonums
 * - Characters
 */
bool cact_val_eqv(struct cact_val l, struct cact_val r);

/* 
 * Compare two values for equality.
 * 
 * `equal?` allows full introspection into compound objects for equality. As such, it can be
 * very expensive on large objects. `equal?` compares the following correctly:
 * 
 * - Pairs, by comparing the car and cdr of the pairs for `equal?`
 * - Vectors, by comparing the corresponding elements of the vectors for `equal?`
 * - Strings, by comparing characters
 */
bool cact_val_equal(struct cact_val l, struct cact_val r);

/*
 * For completeness, marking and destroying arbitrary values is supported, though in most cases it 
 * does nothing.
 * 
 * More info about marking and destroying can be found in the GC implementation in `sweep.h`.
 */
void cact_mark_val(struct cact_val);
void cact_destroy_val(struct cact_val);

#endif // CACT_VAL_H

