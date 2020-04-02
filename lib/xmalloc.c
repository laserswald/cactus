#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *
xrealloc(void *ptr, size_t sz)
{
	void *data = realloc(ptr, sz);
	if (NULL == data) {
		perror("Could not allocate memory");
		abort();
	}
	return data;
}

void *
xmalloc(size_t sz)
{
	void *ptr = xrealloc(NULL, sz);
	return memset(ptr, 0, sz);
}

void
xfree(void *ptr)
{
	if (ptr) {
		free(ptr);
	}
	ptr = NULL;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *data = calloc(nmemb, size);
	if (data == NULL) {
		perror("Could not allocate memory");
		abort();
	}
	return data;
}

void *
xmemdup(void *thing, size_t size)
{
	void *new = xmalloc(size);
	memcpy(new, thing, size);
	return new;
}

char *
xstrdup(const char *str)
{
	size_t sl = strlen(str);
	char *ns = xmalloc(sl + 1);
    memcpy(ns, str, sl);
    ns[sl] = '\0';
    return ns;
}

