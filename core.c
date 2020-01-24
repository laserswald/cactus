#include "globals.h"
#include "core.h"
#include "config.h"
#include "debug.h"
#include "eval.h"
#include "read.h"
#include "sexp.h"
#include "utils.h"
#include "write.h"
#include "proc.h"

void 
cact_make_builtin(cact_env *e, cact_val *x, cact_native_func fn)
{
    cact_val *c = malloc(sizeof(*c));
    c->t = CACT_TYPE_PROCEDURE;
    c->c = malloc(sizeof(cact_proc));
    c->c->nativefn = fn;
    envadd(e, x, c);
}

cact_env* 
cact_make_builtins(struct cact_env *env) 
{
    if_sym.sym = "if";
    define_sym.sym = "define";
    begin_sym.sym = "begin";
    quote_sym.sym = "quote";
    lambda_sym.sym = "lambda";
    set_sym.sym = "set!";

    int i;
    for (i = 0; i < LENGTH(builtins); i++) {
        cact_make_builtin(env, cact_make_symbol(builtins[i].name), builtins[i].fn);
    }

    return env;
}

void cact_init(struct cactus *cact)
{
	cact->root_env = malloc(sizeof(struct cact_env));
    envinit(cact->root_env, NULL);
    cact_make_builtins(cact->root_env);
    cact->current_env = cact->root_env;
    // cact_gc_init(&cact->gc);
}

/* Finalize a cactus interpreter. */
void cact_finish(struct cactus *cact)
{
    // cact_gc_finish(&cact->gc);
}

/* Evaluate a file using the interpreter. */
struct cact_val * cact_eval_file(struct cactus *cact, FILE *in)
{
    DBG("Running file. \n");

    char *s = slurp(in);

    return cact_eval_string(cact, s);
}

/* Evaluate a string using the interpreter. */
struct cact_val * cact_eval_string(struct cactus *cact, char *s)
{
    cact_lexer_init(&cact->lexer, s);

    struct cact_val *x;
    while (*s != '\0') {
        int status = 0;

        DBG("Reading new sexp. \n");

        status = cact_read(&cact->lexer, &x);

        if (status != CACT_READ_OK) {
            switch (status) {

            case CACT_READ_END_OF_FILE:
                goto STOP_RUNNING;

            case CACT_READ_OTHER_ERROR:
                fprintf(stderr, "unknown error\n");
                fprint_sexp(stderr, x);
                abort();
                break;

            }
        }

        DBG("Evaluating sexp: ");
        // print_sexp(x);
        DBG("\n");
        cact_eval(cact, x);
        DBG("Done evaluating sexp.\n");
    }

STOP_RUNNING:

    DBG("Finished running string. \n");
    return 0;
}

