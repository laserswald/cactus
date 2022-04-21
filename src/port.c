#include <stdio.h>

#include "core.h"
#include "str.h"
#include "err.h"
#include "char.h"
#include "storage/obj.h"
#include "port.h"
#include "builtin.h"

struct cact_val
cact_make_input_port(struct cactus *cact, const char* filename)
{
    struct cact_port *p = (struct cact_port *)cact_alloc(cact, CACT_OBJ_PORT);
    p->type = CACT_PORT_INPUT;
    p->cbuf = -1;
    p->file = fopen(filename, "r");
	if (! p->file) {
        return cact_make_error(cact, "Could not open file", cact_make_string(cact, filename));
    }

    return CACT_OBJ_VAL(p);
}

struct cact_val
cact_builtin_open_input_file(struct cactus *cact, struct cact_val args)
{
    struct cact_val filename_val;

    if (1 != cact_unpack_args(cact, args, "s", &filename_val)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    return cact_make_input_port(cact, cact_to_string(filename_val, "open-input-file")->str);
}

struct cact_val
cact_make_output_port(struct cactus *cact, const char* filename)
{
    struct cact_port *p = (struct cact_port *)cact_alloc(cact, CACT_OBJ_PORT);
    p->type = CACT_PORT_OUTPUT;
    p->cbuf = -1;
    p->file = fopen(filename, "w");
    return CACT_OBJ_VAL(p);
}

struct cact_val
cact_builtin_open_output_file(struct cactus *cact, struct cact_val args)
{
    struct cact_val filename_val;

    if (1 != cact_unpack_args(cact, args, "s", &filename_val)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    return cact_make_output_port(cact, cact_to_string(filename_val, "open-output-file")->str);
}

bool
cact_is_input_port(struct cact_val val)
{
	return cact_is_port(val) && cact_to_port(val, "input-port?")->type == CACT_PORT_INPUT;
}
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_input_port, cact_is_input_port)

bool
cact_is_output_port(struct cact_val val)
{
	return cact_is_port(val) && cact_to_port(val, "output-port?")->type == CACT_PORT_OUTPUT;
}
DEFINE_TYPE_PREDICATE_BUILTIN(cact_builtin_is_output_port, cact_is_output_port)

// Read a character, or the EOF object.
struct cact_val
cact_port_read_char(struct cactus *cact, struct cact_port *port)
{
    int c = fgetc(port->file);
    if (c == EOF) {
        return CACT_EOF_VAL;
    }
    return CACT_CHAR_VAL(c);
}

struct cact_val
cact_builtin_read_char(struct cactus *cact, struct cact_val args)
{
    struct cact_val port_val;

    if (1 != cact_unpack_args(cact, args, ".", &port_val)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    return cact_port_read_char(cact, cact_to_port(port_val, "read-char"));
}


// Peek at the character on the port. If the port is empty, returns the EOF object.
struct cact_val
cact_port_peek_char(struct cactus *cact, struct cact_port *port)
{
}

// Write the character to the port.
struct cact_val
cact_port_write_char(struct cactus *cact, struct cact_port *port, char c)
{
    int rc = fputc(c, port->file);
    if (rc == EOF) {
        return CACT_EOF_VAL;
    }
    return CACT_UNDEF_VAL;
}

struct cact_val
cact_builtin_write_char(struct cactus *cact, struct cact_val args)
{
    struct cact_val port_val, char_val;

    if (2 != cact_unpack_args(cact, args, "..", &char_val, &port_val)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    return cact_port_write_char(
	    cact, 
	    cact_to_port(port_val, "read-char"),
	    cact_to_char(char_val, "read-char")
    );
}

void
cact_close_input_port(struct cactus *cact, struct cact_port *port)
{
	fclose(port->file);
	port->file = NULL;
}

struct cact_val
cact_builtin_close_input_port(struct cactus *cact, struct cact_val args)
{
    struct cact_val port_val;

    if (1 != cact_unpack_args(cact, args, ".", &port_val)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    cact_close_input_port(
	    cact,
	    cact_to_port(port_val, "close-input-port")
	);

    return CACT_UNDEF_VAL;
}


void
cact_close_output_port(struct cactus *cact, struct cact_port *port)
{
	fclose(port->file);
	port->file = NULL;
}

struct cact_val
cact_builtin_close_output_port(struct cactus *cact, struct cact_val args)
{
    struct cact_val port_val;

    if (1 != cact_unpack_args(cact, args, ".", &port_val)) {
        return cact_make_error(cact, "Did not get expected number of arguments", args);
    }

    cact_close_output_port(
	    cact,
	    cact_to_port(port_val, "close-output-port")
	);

    return CACT_UNDEF_VAL;
}


