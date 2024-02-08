
#define PRIVATE static

struct cact_record_field_spec {
	struct cact_symbol *name;
	bool is_mutable;
};

ARRAY_DECL(cact_record_field_spec_arr, struct cact_record_field_spec);

struct cact_record_spec {
	struct cact_symbol *name;
	struct cact_record_field_spec_arr *field_specs;
}

/*
 * Construct a constructor function for the record definition.
 */
struct cact_val
cact_make_rec_ctor(struct cactus *cact, struct cact_record_spec *rtd)
{
}

/*
 * Construct a recognizer function for the record definition.
 */
struct cact_val
cact_make_rec_recog(struct cactus *cact, struct cact_record_spec *rtd)
{
}

struct cact_rec_inst {
	struct cact_obj obj;
	struct cact_record_definition *defn;
	struct cact_val_arr fields;
}

struct cact_val
cact_make_rec_inst()
{
}

