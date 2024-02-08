
struct cact_bytecode {
	size_t len;
	size_t alloc;
	uint8_t *bytes;
};

struct cact_irep {
	struct cact_bytecode bytecode;
};

void
cact_bytecode_init(struct cactus *cact, struct cact_bytecode *irep)
{
	irep->len = 0;
	irep->alloc = 10;
	irep->bytes = xcalloc(sizeof uint8_t, irep->alloc);
}

void
cact_emit_byte(struct cactus *cact, struct cact_bytecode *irep, uint8_t byte)
{
	if (irep->len == irep->alloc) {
		size_t oldalloc = irep->alloc;
		irep->alloc *= 1.5;
		irep->bytes = xrealloc(irep->bytes, irep->alloc * (sizeof uint8_t));
	}
	irep->bytes[irep->len] = byte;
	irep->len ++;
}

void
cact_emit_instruction()
{
}

void
cact_compile(struct cactus *cact, struct cact_val expr, struct cact_bytecode *irep)
{
	if (is_literal(expr)) {
		cact_compile_literal(cact, expr, irep);
	} else if (is_definition(expr)) {
		cact_compile_defn(cact, expr, irep);
	} else if (is_assignment(expr)) {
		cact_compile_assignment(cact, expr, irep);
	} else if (is_conditional(expr)) {
		cact_compile_if(cact, expr, irep);
	} else if (is_application(expr)) {
		cact_compile_application(cact, expr, irep);
	}
}

void
cact_compile_defn(struct cactus *cact, struct cact_val expr, struct cact_bytecode *irep)
{
	cact_compile(cact, cact_cadr(expr));
	cact_compile(cact, cact_caddr(expr));
	cact_emit_instruction(cact, irep, CACT_IREP_DEFINE);
}

void
cact_compile_assignment(struct cactus *cact, struct cact_val expr, struct cact_bytecode *irep)
{
	cact_compile(cact, cact_cadr(expr));
	cact_compile(cact, cact_caddr(expr));
	cact_emit_instruction(cact, irep, CACT_IREP_ASSIGN);
}

void
cact_compile_if(struct cactus *cact, struct cact_val expr, struct cact_bytecode *irep)
{
	// Compile the conditional expression.
	cact_compile(cact, condition, irep);

	// Jump to the alternative if the condition is false...
	cact_emit_instruction(cact, irep, CACT_IREP_JFALSE);
	struct cact_backpatch begin_of_alt = cact_start_patch(cact, irep);

    // Otherwise, execute the consequent and jump over the alternative
	cact_compile(cact, consequent, irep);
	cact_emit_instruction(cact, irep, CACT_IREP_JUMP);
	struct cact_backpatch end_of_alt = cact_start_patch(cact, irep);

	// Backpatch to the conditional jump
    cact_do_patch(cact, irep, begin_of_alt);

    // Execute the alternative.
	cact_compile(cact, alternative, irep);

	// Backpatch for jumping over the alternative
    cact_do_patch(cact, irep, end_of_alt);
}

void
cact_compile_literal(struct cactus *cact, struct cact_val expr, struct cact_bytecode *irep)
{
	// If this value can be inlined (unlikely)
	if (cact_val_is_inlineable()) {
		cact_emit_inlineable_value(cact, irep, expr);
	} else {
		cact_emit_instruction(cact, irep, CACT_INST_PUSHL);
		int local_id = cact_irep_push_local(cact, irep, expr)
		cact_emit_byte(cact, irep, local_id);
	}
}

void
cact_compile_application()
{
	operation = 
	if (cact_operation_is_builtin()) {
		cact_compile_builtin();
		return;
	}
}

void
cact_compile_builtin()
{
	if 
}
