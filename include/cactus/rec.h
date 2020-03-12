#ifndef CACT_REC_H
#define CACT_REC_H

/* An instance of a record. */
struct cact_record_inst {
	/* The record spec */
	struct cact_record_spec *spec;
	struct cact_val *values;
};

#endif /* CACT_REC_H */
