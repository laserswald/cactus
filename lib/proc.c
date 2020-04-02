
#include "cactus/proc.h"

#include "cactus/core.h"
#include "cactus/val.h"
#include "cactus/pair.h"
#include "cactus/store.h"
#include "cactus/write.h"
#include "cactus/eval.h"
#include "cactus/internal/debug.h"

/* Create a procedure. */
struct cact_val
cact_make_procedure(struct cactus *cact, struct cact_env *e, struct cact_val argl, struct cact_val body)
{
	cact_preserve(cact, (struct cact_obj *) e);
	cact_preserve(cact, argl.as.object);
	cact_preserve(cact, body.as.object);
	struct cact_proc *proc = (struct cact_proc *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
	proc->env = e;
	proc->argl = argl;
	proc->body = body;
	cact_unpreserve(cact);
	cact_unpreserve(cact);
	cact_unpreserve(cact);
	return CACT_OBJ_VAL((struct cact_obj *) proc);
}

struct cact_val
cact_make_native_proc(struct cactus *cact, cact_native_func fn)
{
	struct cact_proc *nat = (struct cact_proc *)cact_alloc(cact, CACT_OBJ_PROCEDURE);
	nat->nativefn = fn;
	return CACT_OBJ_VAL((struct cact_obj *) nat);
}

static struct cact_env *
cact_proc_eval_args(struct cactus *cact, struct cact_env *parent_env, struct cact_val params, struct cact_val args)
{
	struct cact_env *params_env;
	struct cact_val current_arg;

	params_env = (struct cact_env *) cact_alloc(cact, CACT_OBJ_ENVIRONMENT);
	cact_env_init(params_env, parent_env);

	current_arg = args;
	CACT_LIST_FOR_EACH_ITEM(cact, param, params) {
		if (cact_is_null(current_arg)) {
			fprintf(stderr, "Not enough arguments!");
			fprint_sexp(stderr, args);
			abort();
		}

		struct cact_val evaled_arg = cact_eval(cact, cact_car(cact, current_arg));

        printf("eval_args: evaluated arg: ");
		print_sexp(evaled_arg);
		printf("\n");

		cact_env_define(
		    cact,
		    params_env,
		    cact_to_symbol(param, "cact_proc_eval_args"),
		    evaled_arg
		);

		current_arg = cact_cdr(cact, current_arg);
	}

	return params_env;
}

/* Apply a procedure given the arguments and the environment. */
struct cact_val
cact_proc_apply(struct cactus *cact, struct cact_proc *clo, struct cact_val args)
{
	if (clo->nativefn != NULL) {
		// TODO: evaluate args then pass the list
		return clo->nativefn(cact, args);
	} else {
		assert(clo->nativefn == NULL);
		assert(clo->argl.as.object != args.as.object);
		cact_call_stack_push(cact, cact_proc_eval_args(cact, clo->env, clo->argl, args));
		cact_show_call_stack(cact);
		struct cact_val ret = cact_eval_list(cact, clo->body);
		cact_call_stack_pop(cact);
		return ret;
	}
}

void
cact_mark_proc(struct cact_obj *o)
{
	struct cact_proc *p = (struct cact_proc *) o;

	if (p->nativefn) {
		return;
	}

	puts("; cactus gc: marking procedure");

    if (p->env) {
		cact_obj_mark((struct cact_obj *) p->env);
    }

	puts("marking arg list");
	print_sexp(p->argl);
	fflush(stdout);
	cact_mark_val(p->argl);
	cact_mark_val(p->body);
}

void
cact_destroy_proc(struct cact_obj *o)
{
	return;
}

