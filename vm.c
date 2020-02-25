
enum cactus_vm_op {
#define OP_DO(enum, namestr, imm_bytes) enum,
#include "operations.inc"
#undef OP_DO
};

struct cactus_op_defn {

};

static
struct cactus_op_defn op_defs = {
#define OP_DO(enum, namestr, imm_bytes, ) \
    [enum] = {}
#include "operations.inc"
#undef OP_DO
};

static
op_nop(struct cactus_vm *vm, )

