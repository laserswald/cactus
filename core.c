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
    cact_val c;
    c->t = CACT_TYPE_PROCEDURE;
    c->c = malloc(sizeof(cact_proc));
    c->c->nativefn = fn;
    envadd(e, x, c);
}

cact_env* 
cact_make_builtins(struct cactus *cact) 
{
    int i;
    for (i = 0; i < LENGTH(builtins); i++) {
        struct cact_val *sym = cact_get_symbol(cact, builtins[i].name);
        cact_make_builtin(cact->root_env, sym, builtins[i].fn);
    }

    return cact->root_env;
}

void cact_init(struct cactus *cact)
{
	// Begin by initializing the symbol table
	TABLE_INIT(&cact->interned_syms);

	cact_store_init(&cact->store);

	cact->root_env = malloc(sizeof(struct cact_env));
	envinit(cact->root_env, NULL);
	cact_make_builtins(cact);

	cact->current_env = cact->root_env;
}

/* Finalize a cactus interpreter. */
void cact_finish(struct cactus *cact)
{
    // cact_gc_finish(&cact->gc);
}

/* Evaluate a file using the interpreter. */
struct cact_val
cact_eval_file(struct cactus *cact, FILE *in)
{
    DBG("Running file. \n");
    return cact_eval_string(cact, slurp(in));
}

/* Evaluate a string using the interpreter. */
struct cact_val 
cact_eval_string(struct cactus *cact, char *s)
{
    struct cact_val exp = CACT_NULL_VAL;
    struct cact_val ret = CACT_NULL_VAL;

    cact_lexer_init(&cact->lexer, s);

    while (*s != '\0') {
        int status = 0;
        DBG("Reading new sexp. \n");

        status = cact_read(cact, &exp);
        if (status != CACT_READ_OK) {
            switch (status) {

            case CACT_READ_END_OF_FILE:
                goto STOP_RUNNING;

            case CACT_READ_UNMATCHED_CHAR:
                fprint_sexp(stderr, exp);
                return ret;

            case CACT_READ_OTHER_ERROR:
                fprintf(stderr, "unknown error\n");
                fprint_sexp(stderr, exp);
                break;
            }
        }

        ret = cact_eval(cact, exp);
    }
STOP_RUNNING:
    return ret;
}

