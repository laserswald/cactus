#include <stdlib.h>
#include <stdint.h>

enum cact_instruction
{
    CACT_INST_HALT,

    //
    // Function calls and other control flow
    //

    CACT_INST_RETURN, // Return values
    CACT_INST_CALL, // Call a function

    // Jumping
    CACT_INST_JUMP,
    CACT_INST_JFALSE,
    CACT_INST_JTRUE,
    CACT_INST_JZERO,

    // Data
    CACT_INST_PUSHCC, // Push the current continuation
    CACT_INST_PLOCAL,  // Push a local with the given index
    CACT_INST_PGLOBAL,  // Push a global with the given index
    CACT_INST_TRUE,  // Push true
    CACT_INST_FALSE,  // Push false
    CACT_INST_NULL,  // Push null
    CACT_INST_ZERO,  // Push zero
    CACT_INST_ONE,  // Push one

    // Logic
    CACT_INST_AND,
    CACT_INST_OR,
    CACT_INST_NOT,
    CACT_INST_ZEROP,

    // Math
    CACT_INST_ADD,
    CACT_INST_SUB,
    CACT_INST_MUL,
    CACT_INST_DIV,
};

struct cact_exec_chunk {
    struct cact_val_arr constants;
	struct cact_bytevector bytecode;
};

struct cact_executor {
	struct cact_bytevector bytecode;
    // Instruction pointer
    uint8_t *ip;

    struct cact_val_arr stack;
    int sp;
};

struct cact_val
cact_executor_pop_val(struct cact_executor *exec)
{

}

struct cact_val
cact_executor_push_val(struct cact_executor *exec, struct cact_val v)
{
}

struct cact_val
cact_executor_peek_val(struct cact_executor *exec)
{
}

#define CHECK(expr) \
    if (!(expr)) abort()

void
cact_executor_init(struct cact_executor *exec)
{
    CHECK(exec);
}

static inline uint8_t
cact_exec_pop_byte(struct cact_executor *exec)
{
}

enum cact_execute_result
cact_execute(struct cactus *cact, struct cact_executor *exec)
{
	#define 
	for (;;) {
        uint8_t instruction;
        switch (instruction = cact_exec_pop_byte(exec)) {
		case CACT_INST_HALT:
			return;

	    case CACT_INST_RETURN: {
			uint8_t amount = cact_exec_pop_short(exec);
		    return CACT_EXECUTE_OK;
        }

	    case CACT_INST_CALL: {
			uint8_t argc = cact_exec_pop_short(exec);
			
        }

	    case CACT_INST_JUMP: {
			uint8_t offset = cact_exec_pop_short(exec);
			vm.ip += offset;
			break;
        }

	    case CACT_INST_JFALSE: {
	    }

	    case CACT_INST_JTRUE: {
	    }

	    case CACT_INST_JZERO: {

	    }

	    case CACT_INST_PUSHC:
	    case CACT_INST_PLOCAL:
	    case CACT_INST_PGLOBAL:

	    case CACT_INST_TRUE: {

	    }
	    case CACT_INST_FALSE:
	    case CACT_INST_NULL:
	    case CACT_INST_ZERO:
	    case CACT_INST_ONE:
	    case CACT_INST_AND:
	    case CACT_INST_OR:
	    case CACT_INST_NOT:
	    case CACT_INST_ZEROP:
        }
	}
}
