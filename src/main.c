/* See LICENSE for license details. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "core.h"
#include "evaluator/eval.h"
#include "write.h"
#include "err.h"

#include "internal/utils.h"
#include "linenoise.h"

#include "../config.h"

int verbosity = 0;

/* macros */
#define TTYGREEN "\x1b[32m"
#define TTYCLEAR "\x1b[0m"

#define PROMPT ">()< "

int
repl(cact_context_t *cact)
{
    char *line;
    cact_value_t x;

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

cact_value_t
load_file(cact_context_t *cact, char *filename)
{
    cact_value_t v;
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

int
main(int argc, char *argv[])
{
    cact_context_t cact;
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
