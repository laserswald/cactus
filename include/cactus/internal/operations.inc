_INST('nop', NOP, 0)
_INST('call', CALL, 1)
_INST('tcall', TCALL, 1)
_INST('ret', RET, 1)
_INST('typep', TYPEP, 2)

_INST('jmp', JMP, 1)
_INST('jz', JZ, 1)
_INST('jlt', JLT, 1)
_INST('jle', JLE, 1)

_INST('eq', EQ, 2)
_INST('eqv', EQV, 2)
_INST('eql', EQL, 2)

_INST('add', ADD, 2)
_INST('sub', SUB, 2)
_INST('mul', MUL, 2)
_INST('div', DIV, 2)

_INST('cons', CONS, 2)
_INST('car', CAR, 2)
_INST('cdr', CDR, 2)
_INST('scar', SCAR, 2)
_INST('scdr', SCDR, 2)

_INST('vec', VEC, 2)
_INST('vref', VREF, 2)
_INST('vset', VSET, 2)
_INST('vlen', VLEN, 2)

_INST('sym', SYM, 1)

