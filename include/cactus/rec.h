#ifndef CACT_REC_H
#define CACT_REC_H

#include "cactus/obj.h"
#include "cactus/sym.h"

struct cact_record_spec {
	struct cact_obj         obj;
	struct cact_record_spec *parent;
	struct cact_symbol      *name;
	struct cact_symbol_arr  fields;
};

unsigned int cact_record_spec_field_count(struct cact_record_spec *);

/* An instance of a record. */
struct cact_record_inst {
	struct cact_obj obj;
	struct cact_record_spec *spec;
	struct cact_val_arr     *values;
};

struct cact_val cact_make_record(struct cactus *, struct cact_record_spec *);

#endif /* CACT_REC_H */
