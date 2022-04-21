#ifndef __CACT_PORT_H__
#define __CACT_PORT_H__

enum cact_port_type {
	CACT_PORT_INPUT,
	CACT_PORT_OUTPUT
};

struct cact_port {
	struct cact_obj obj;
	enum cact_port_type type;
	FILE *file;
	int cbuf;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_PORT, struct cact_port*, cact_to_port, port)
DEFINE_OBJECT_CHECK(cact_is_port, CACT_OBJ_PORT)

struct cact_val
cact_make_input_port(struct cactus *cact, const char* filename);

struct cact_val
cact_make_output_port(struct cactus *cact, const char* filename);

bool
cact_is_input_port(struct cact_val val);

bool
cact_is_output_port(struct cact_val val);

struct cact_val
cact_port_read_char(struct cactus *cact, struct cact_port *port);

struct cact_val
cact_port_peek_char(struct cactus *cact, struct cact_port *port);

struct cact_val
cact_port_write_char(struct cactus *cact, struct cact_port *port, char c);

void
cact_close_input_port(struct cactus *cact, struct cact_port *port);

#endif /* __CACT_PORT_H__ */
