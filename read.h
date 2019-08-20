#ifndef read_h_INCLUDED
#define read_h_INCLUDED

#include "sexp.h"

enum {
	READSEXP_OK,
	READSEXP_END_OF_FILE,
	READSEXP_OTHER_ERROR,
};

struct lexer;

void lexer_init(struct lexer *l, const char* data);

int readsexp(struct lexer *l, Sexp **s);

#endif // read_h_INCLUDED

