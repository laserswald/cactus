#ifndef HEAP_H
#define HEAP_H

/* Garbage collector context. */
struct cact_gc;

/* Intrusive data structure for GC. Should be named `gc_entry` */
struct cact_gc_entry;

/* Protect a sexp from collection. */
void cact_gc_protect(struct cact_gc *, struct sexp *);

/* Allow a sexp to be collected. */
void cact_gc_yield(struct cact_gc *, struct sexp *);

/* Allocate a new sexp. */
struct sexp *cact_gc_alloc(struct cact_gc *);

#endif /* HEAP_H */
