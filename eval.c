#include "debug.h"
#include "sexp.h"
#include "sym.h"
#include "env.h"
#include "eval.h"
#include "builtin.h"
#include "globals.h"
#include "utils.h"

/* Apply a closure given the arguments and the environment. */
Sexp *
apply(Closure *clo, Sexp *args, Env *e)
{	
	if (clo->nativefn) {
		return clo->nativefn(args, e);
	} else {
		Env params_env = {0};
		envinit(&params_env, clo->env);
		Sexp *current_arg = args;
		LIST_FOR_EACH(clo->argl, param) {
            envadd(&params_env, car(param), eval(car(current_arg), e));
            current_arg = cdr(current_arg);
		}
		return builtin_progn(clo->body, &params_env);
	}
}

void special_define(Sexp* args, Env* e) {
	Sexp *term = car(args);
	Sexp *defn = cdr(args);

    if (is_sym(term)) {
		envadd(e, term, eval(cdr(args), e));
    } else if (is_pair(term)) {
	    Sexp* name = car(term);
	    Sexp* params = cdr(term);
	    Sexp* proc = make_closure(e, params, defn);
		envadd(e, name, proc);
    }
}

/* Evaluate an expression according to an environment. */
Sexp *
eval(Sexp *x, Env *e)
{
	if (!x) return NULL;

	switch (x->t) {
		case TYPE_INT:
		case TYPE_DOUBLE:
		case TYPE_STRING:
		case TYPE_CLOSURE:
		case TYPE_ENVIRONMENT:
			DBG("Evaluating self-evaluating atom %s\n", show_type(x->t));
			return x;	

		case TYPE_SYMBOL: {
			Sexp *found = envlookup(e, x);
			if (!found) {
				return make_error("No such symbol", x);
			}
			return cdr(found);
		}

		case TYPE_PAIR: {
			Sexp *op = car(x);

			if (! op) {
				return make_error("Cannot evaluate null as operation", x);
			}
			if (! is_sym(op)) {
				return make_error("Non symbol as operator", car(x));
			}

			Symbol sym = to_sym(op, "eval");

			Sexp *arglist = cdr(x);

			if (symcmp(&quote_sym, &sym) == 0) {
				return cdr(x);
			} else if (symcmp(&if_sym, &sym) == 0) {
				Sexp* ret = builtin_if(cdr(x), e);
				return ret;
			} else if (symcmp(&define_sym, &sym) == 0) {
				special_define(arglist, e);
				return &undefined;
			} else if (symcmp(&lambda_sym, &sym) == 0) {
				Sexp *args = car(arglist);
				Sexp *body = cdr(arglist);
				return make_closure(e, args, body);
			} else if (symcmp(&progn_sym, &sym) == 0) {
				return builtin_progn(cdr(x), e);
			} else {
				Sexp *maybe_closure = eval(op, e);
				if (! is_closure(maybe_closure)) {
					fprintf(stdout, "Cannot apply non-operation %s:\n", show_type(maybe_closure->t));
					print_sexp(maybe_closure);
					abort();
				}
				return apply(maybe_closure->c, arglist, e);
			}
		}

		case TYPE_ERROR: {
			fprintf(stderr, "Error! %s", x->x.msg);
			print_list(x->x.ctx);
			fprintf(stderr, "\n");
			abort();
		}

		default:
			fprintf(stderr, "Cannot evaluate, got type %s.\n", show_type(x->t));
			abort();
	}
}
