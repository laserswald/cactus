#ifndef __CACTUS_VM__
#define __CACTUS_VM__

#define _INST(name, enum, numargs) CACT_INSTR_##enum,
enum cact_vm_instr {
	#include "internal/operations.inc"
};
#undef _INST

#define _INST(nm, enm, ari) [CACT_INSTR_##enm] = {.name = nm, .arity = ari},
struct cact_vm_instr_info {
	const char *name;
	unsigned int arity;
} cact_ops_info [] = {
	#include "internal/operations.inc"
}
#undef _INST

ARRAY_DECL(cact_bytecode, uint8_t)

struct cact_vm {
	struct cact_bytecode *running;
	unsigned int ip;
	unsigned int sp;
};

void cact_vm_init(struct cact_vm *);
void cact_vm_finish(struct cact_vm *);
void cact_vm_execute(struct cact_vm *, struct cact_bytecode *);

#endif /* __CACTUS_VM__ */

