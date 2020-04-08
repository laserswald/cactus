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

#include "config.h"

int verbosity = 0;

/* macros */

#define PROMPT "cactus> "

int
repl(struct cactus *cact, FILE *f)
{
    char line[256];
    struct cact_val x;

    printf(PROMPT);
    while (fgets(line, sizeof line, f) != NULL)
    {
        x = cact_eval_string(cact, line);
        cact_display(x);
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

    cact_init(&cact);

    cact_define_builtin_array(&cact, builtins, LENGTH(builtins));

    if (argc == 2)
    {
        infile = fopen(argv[1], "r");
        if (! infile)
        {
            perror("Could not run file");
            exit(1);
        }
        struct cact_val ret = cact_eval_file(&cact, infile);
        if (cact_is_error(ret))
        {
            cact_display(ret);
            perror("Encountered errors");
            exit(1);
        }
    }
    else
    {
        result = repl(&cact, stdin);
    }

    cact_finish(&cact);

    return result;
}
