
TABLE_DECL(cact_val_table, struct cact_val)

struct cact_comparator {
	struct cact_obj obj;

};

enum cact_hashtable_type {
	CACT_HASHTABLE_EQ,
	CACT_HASHTABLE_EQV,
	CACT_HASHTABLE_EQUAL,
	CACT_HASHTABLE_COMPARATOR,
};

struct cact_hashtable {
	struct cact_obj obj;
	enum cact_hashtable_type type;
	struct cact_val_table table;
	struct cact_proc *cmp;
};

struct cact_val
cact_make_hashtable(struct cactus *cact)
{
	struct cact_hashtable *tab = 
		(struct cact_hashtable *) cact_alloc(cact, CACT_OBJ_HASHTABLE);

	return CACT_OBJ_VAL(tab);
}

struct cact_val
cact_hashtable_ref(struct cactus *cact, struct cact_hashtable *tab
                   struct cact_val key)
{
	if (TABLE_HAS(cact_val_table, tab->table, key)) {
		return TABLE_FIND(cact_val_table, tab->table, key);
	}
	return CACT_BOOL_VAL(false);
}

void
cact_hashtable_set(struct cactus *cact, struct cact_hashtable *tab,
                   struct cact_val key, struct cact_val val)
{
	TABLE_ENTER(cact_val_table, tab->table, key);
	return CACT_UNDEF_VAL;
}

bool
cact_hashtable_has(struct cactus *cact, struct cact_hashtable *tab
                   struct cact_val key)
{
	return TABLE_HAS(cact_val_table, tab->table, key);
}
