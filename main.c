/* See LICENSE for license details. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "globals.h"
#include "debug.h"
#include "sexp.h"
#include "utils.h"
#include "xmalloc.h"
#include "read.h"
#include "write.h"
#include "env.h"
#include "sym.h"
#include "eval.h"
#include "builtin.h"
#include "load.h"

int verbosity = 0;

/* macros */

void 
dbg(Sexp *x)
{
	printf("; debug: ");
	print_sexp(x);
	puts("");
}



int
repl(FILE *f, Env *e)
{
	char line[256];
	Sexp *x;
	int status;
	struct lexer l;

	printf("slisp> ");
	while (fgets(line, sizeof line, f) != NULL) {
		char *lp = line;
		lexer_init(&l, lp);
		status = readsexp(&l, &x);
		if (status != READSEXP_OK) {
			fprintf(stderr, "Could not finish reading sexp!\n");
			fprint_sexp(stderr, x);
			abort();
		}
		x = eval(x, e);

		print_sexp(x);
		printf("\nslisp> ");
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	DBG("Starting.\n");
	Env *e = make_builtins();
	FILE *infile = NULL;

	if (argc == 2) {
		DBG("Reading file.\n");
		infile = fopen(argv[1], "r");
		if (! infile) {
			perror("Could not run file");
			exit(1);
		}
		return runfile(infile, e);
	}

	DBG("Starting REPL.\n");
	return repl(stdin, e);
}
