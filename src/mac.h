#ifndef CACT_MAC_H
#define CACT_MAC_H

struct cact_macro {
	struct cact_obj obj;
	enum cact_macro_type type;
	union {
		struct cact_proc *unhygenic;
	} as;
};

struct cact_val
cact_make_unhygenic_macro(struct cactus *, struct cact_proc *);

struct cact_val
cact_expand_macro(struct cactus *, struct cact_macro *, struct cact_val);

#endif
