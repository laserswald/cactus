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
runfile(FILE *in, Env *e)
{
	DBG("Running file. \n");

	char *s = slurp(in);

	struct lexer l;
	lexer_init(&l, s);

	Sexp *x;
	while (*s != '\0') {
		int status = 0;

		DBG("Reading new sexp. \n");

		status = readsexp(&l, &x);

		if (status != READSEXP_OK) {
			switch (status) {
				case READSEXP_END_OF_FILE: {
					goto STOP_RUNNING;
				}
				case READSEXP_OTHER_ERROR: {
					fprintf(stderr, "unknown error\n");
		            abort();
					break;
				}
			}
		}

		DBG("Evaluating sexp: ");
		// print_sexp(x);
		DBG("\n");
		eval(x, e);
		DBG("Done evaluating sexp.\n");
	}

STOP_RUNNING:

	DBG("Finished running file. \n");
	return 0;
}

int
repl(FILE *f, Env *e)
{
	char line[256];
	Sexp *x;
	int status;
	struct lexer l;

	printf("cactus> ");
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
		printf("\ncactus> ");
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
