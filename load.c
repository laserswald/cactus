#include "read.h"
#include "sexp.h"
#include "debug.h"
#include "utils.h"
#include "eval.h"

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

        status = cact_read(&l, &x);

        if (status != CACT_READ_OK) {
            switch (status) {
            case CACT_READ_END_OF_FILE: {
                goto STOP_RUNNING;
            }
            case CACT_READ_OTHER_ERROR: {
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

