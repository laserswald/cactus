/* See LICENSE for license details. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "core.h"
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

#define PROMPT "cactus> "

void
dbg(cact_val *x)
{
    printf("; debug: ");
    print_sexp(x);
    puts("");
}

int
repl(struct cactus *cact, FILE *f)
{
    char line[256];
    struct cact_val *x;

    printf(PROMPT);
    while (fgets(line, sizeof line, f) != NULL) {
        x = cact_eval_string(cact, line);
        print_sexp(x);
        puts("");
        printf(PROMPT);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct cactus cact;
    int result = EXIT_SUCCESS;
    FILE *infile = NULL;

    DBG("Starting.\n");

    cact_init(&cact);

    if (argc == 2) {
        DBG("Reading file.\n");
        infile = fopen(argv[1], "r");
        if (! infile) {
            perror("Could not run file");
            exit(1);
        }
        struct cact_val *ret = cact_eval_file(&cact, infile);
        if (is_error(ret)) {
	        print_sexp(ret);
            perror("Encountered errors");
            exit(1);
        }
    } else {
	    DBG("Starting REPL.\n");
	    result = repl(&cact, stdin);
    }

    cact_finish(&cact);

    return result;
}
