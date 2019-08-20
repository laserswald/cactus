#include "write.h"

void 
print_list(Sexp *x)
{
	printf("(");
	Sexp *p = x;
	while (p != NULL) {
		if (is_pair(p)) {
			print_sexp(car(p));
			printf(" ");
			p = cdr(p);
		} else {
			printf(". ");
			print_sexp(p);
			break;
		}
	}
	printf(")");
}

void
print_sexp(Sexp *x)
{
	if (!x) {
		printf("()");
		return;
	}

	switch (x->t) {
		case TYPE_INT:
			printf("%i", x->i);
			break;
		case TYPE_DOUBLE:
			printf("%f", x->f);
			break;
		case TYPE_STRING:
			printf("%s", x->s.str);
			break;
		case TYPE_SYMBOL:
			printf("#<symbol: '%s'>", x->a.sym);
			break;
		case TYPE_PAIR:
			print_list(x);
			break;
		case TYPE_CLOSURE:
			printf("#<closure: %p>", (void*)x->c);
			break;
		case TYPE_ENVIRONMENT:
			printf("#<environment: %p>", (void*)x->e);
			break;
		case TYPE_PORT:
			break;
		case TYPE_ERROR:
			printf("; error '%s' : ", x->x.msg);
			print_sexp(x->x.ctx);
			break;
		case TYPE_UNDEF:
			printf("UNDEFINED");
			break;
	}
}
