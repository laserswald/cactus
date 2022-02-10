#ifndef XMALLOC_H
#define XMALLOC_H

#include <stddef.h>

/* wrappers for malloc that prevent OOM errors */

void *xrealloc(void *ptr, size_t sz);
void *xmalloc(size_t sz);
void xfree(void *ptr);
void *xcalloc(size_t nmemb, size_t size);
void *xmemdup(void *ptr, size_t size);
char *xstrdup(const char *);

#endif

