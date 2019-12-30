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

#define PROMPT "cactus> "

void
dbg(cact_val *x)
{
    printf("; debug: ");
    print_sexp(x);
    puts("");
}

int
repl(FILE *f, cact_env *e)
{
    char line[256];
    cact_val *x;
    int status;
    struct cact_lexer l;

    printf(PROMPT);
    while (fgets(line, sizeof line, f) != NULL) {
        char *lp = line;
        cact_lexer_init(&l, lp);
        status = cact_read(&l, &x);
        if (status != CACT_READ_OK) {
            fprintf(stderr, "Could not finish reading sexp!\n");
            fprint_sexp(stderr, x);
            abort();
        }
        x = cact_eval(x, e);
        print_sexp(x);
        puts("");
        printf(PROMPT);
    }

    return 0;
}

int
main(int argc, char *argv[])
{
    DBG("Starting.\n");
    cact_env *e = cact_make_builtins();
    FILE *infile = NULL;

    if (argc == 2) {
        DBG("Reading file.\n");
        infile = fopen(argv[1], "r");
        if (! infile) {
            perror("Could not run file");
            exit(1);
        }
        return cact_load(infile, e);
    }

    DBG("Starting REPL.\n");
    return repl(stdin, e);
}
