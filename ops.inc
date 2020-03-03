
#ifndef DO_OP
#error "This file should be included after you define DO_OP."
#else
	#define CACT_OP_N_ARGS -1
    DO_OP(CACT_OP_NOP, "nop", 0)

    DO_OP(CACT_OP_CALL, "call", 1)
    DO_OP(CACT_OP_TCALL, "tcall", 1)
    DO_OP(CACT_OP_RET, "return", 1)
    DO_OP(CACT_OP_JMP, "jmp", 1)
    DO_OP(CACT_OP_JZ, "jz", 1)

    DO_OP(CACT_OP_PUSHL, "pushl", 1)
    DO_OP(CACT_OP_SETL, "setl", 1)

    DO_OP(CACT_OP_TYPEP, "typep", 1)  

    DO_OP(CACT_OP_ADD, "add", 2)
    DO_OP(CACT_OP_SUB, "sub", 2)
    DO_OP(CACT_OP_MUL, "mul", 2)
    DO_OP(CACT_OP_DIV, "div", )
    DO_OP(CACT_OP_NEG, "neg", 1)
    DO_OP(CACT_OP_MOD, "mod", )
    DO_OP(CACT_OP_POW, "pow", )

    DO_OP(CACT_OP_CONS, "cons", 2)
    DO_OP(CACT_OP_CAR, "car", 1)
    DO_OP(CACT_OP_CDR, "cdr", 1)
    DO_OP(CACT_OP_SETCAR, "setcar", 2)
    DO_OP(CACT_OP_SETCDR, "setcdr", 2)

    DO_OP(CACT_OP_VEC, "vec", 1)
    DO_OP(CACT_OP_VECREF, "vecref", 2)
#endif
