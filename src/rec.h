#ifndef CACT_REC_H
#define CACT_REC_H

#include "storage/obj.h"
#include "sym.h"

typedef struct cact_record_spec {
	cact_object_t obj;
	struct cact_record_spec *parent;
	cact_symbol_t      *name;
	cact_symbol_array_t  fields;
} cact_record_spec_t;

unsigned int 
cact_record_spec_field_count(cact_record_spec_t *);

/* An instance of a record. */
struct cact_record_inst {
	cact_object_t      obj;
	cact_record_spec_t *spec;
	cact_value_array_t   *values;
};

cact_value_t 
cact_make_record(cact_context_t *, cact_record_spec_t *);

#endif /* CACT_REC_H */
