/* See LICENSE for license details. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "cactus/core.h"
#include "cactus/eval.h"
#include "cactus/write.h"
#include "cactus/err.h"

#include "cactus/internal/utils.h"
#include "linenoise.h"

#include "../config.h"

int verbosity = 0;

/* macros */
#define TTYGREEN "\x1b[32m"
#define TTYCLEAR "\x1b[0m"

#define PROMPT "cact> "

int
repl(struct cactus *cact)
{
    char *line;
    struct cact_val x;

    while ((line = linenoise(PROMPT))!= NULL) {
        x = cact_eval_string(cact, line);
        cact_display(x);
        putchar('\n');
        fflush(stdout);
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }

    return 0;
}

struct cact_val
load_file(struct cactus *cact, char *filename)
{
	struct cact_val v;
	FILE *f = NULL;

    f = fopen(filename, "r");
    if (! f) {
	    perror("Could not open file");
	    exit(1);
    }

    v = cact_eval_file(cact, f);

    if (cact_is_error(v)) {
        cact_display(v);
        perror("Encountered errors");
        exit(1);
    }

    return v;
}

int main(int argc, char *argv[])
{
    struct cactus cact;
    int result = EXIT_SUCCESS;

    cact_init(&cact);

    cact_define_builtin_array(&cact, builtins, LENGTH(builtins));

    load_file(&cact, "rt/init.scm");

    if (argc == 2) {
        load_file(&cact, argv[1]);
    } else {
        result = repl(&cact);
    }

    cact_finish(&cact);

    return result;
}
