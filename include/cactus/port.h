
struct cact_string_port {
	char *string;
	size_t length;
    size_t cursor;
};

struct cact_file_port {
	FILE *file;
	char *filename;
};

struct cact_port {
	struct cact_obj obj;
	enum cact_port_type type;
	union {
		struct cact_string_port string;
		struct cact_file_port file;
	} as;
};

/*
 * Create a port from a string.
 * 
 * The responsibility for freeing the string is moved to the port.
 */
struct cact_val
cact_make_string_port(struct cactus *cact, char *string);

cact_builtin_fn cact_builtin_open_input_string;
DEFINE_BUILTIN("open-input-string", cact_builtin_open_input_string);

struct cact_val
cact_make_file_port(struct cactus *cact, char *filename, bool is_input);

cact_builtin_fn cact_builtin_open_input_file;
DEFINE_BUILTIN("open-input-file", cact_builtin_open_input_file);

void
cact_destroy_port();

struct cact_val
cact_port_read_char(struct cact_port *port);

cact_builtin_fn cact_builtin_read_char;
DEFINE_BUILTIN("read-char", cact_builtin_read_char);

struct cact_val
cact_port_peek_char(struct cact_port *port);

void
cact_port_write_char(struct cact_port *port);

