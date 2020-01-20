#include "core.h"
#include "debug.h"
#include "eval.h"
#include "read.h"
#include "sexp.h"
#include "utils.h"
#include "write.h"

struct specials_table {
    cact_symbol *sym;
    cact_val* (*fn)(cact_val*, cact_env*);
} specials[] = {
    {&quote_sym,  special_quote},
    {&if_sym,     special_if},
    {&define_sym, special_define},
    {&lambda_sym, special_lambda},
    {&set_sym,    special_set_bang},
    {&begin_sym,  special_begin}
};

void 
cact_make_builtin(cact_env *e, cact_val *x, cact_val *(fn)(cact_val*, cact_env*))
{
    cact_val *c = malloc(sizeof(*c));
    c->t = CACT_TYPE_PROCEDURE;
    c->c = malloc(sizeof(cact_proc));
    c->c->nativefn = fn;
    envadd(e, x, c);
}

cact_env* 
cact_make_builtins() 
{
    cact_env *env = malloc(sizeof(cact_env));
    envinit(env, NULL);

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
	create_initial_env()
    cact_gc_init(&cact->gc);
}

/* Finalize a cactus interpreter. */
void cact_finish(struct cactus *)
{
    cact_gc_finish(&cact->gc);
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

        status = cact_read(&l, &x);

        if (status != CACT_READ_OK) {
            switch (status) {
            case CACT_READ_END_OF_FILE: {
                goto STOP_RUNNING;
            }
            case CACT_READ_OTHER_ERROR: {
                fprintf(stderr, "unknown error\n");
                fprint_sexp(stderr, x);
                abort();
                break;
            }
            }
        }

        DBG("Evaluating sexp: ");
        // print_sexp(x);
        DBG("\n");
        cact_eval(x, cact->e);
        DBG("Done evaluating sexp.\n");
    }

STOP_RUNNING:

    DBG("Finished running string. \n");
    return 0;
}

void cact_push_frame(struct cactus *);
void cact_pop_frame(struct cactus *);
struct cact_stack_frame * cact_current_frame(struct cactus *);

