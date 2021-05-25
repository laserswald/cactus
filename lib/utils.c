#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "cactus/internal/xmalloc.h"

void die(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    abort();
}

// Get all the contents of a file.
char* slurp(FILE *f)
{
    char *slurped = NULL;
    size_t slurplen = 0;
    // 64 is normal cache line size, very fast hopefully
    char buffer[64] = {0};

    // go linewise
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        slurped = xrealloc(slurped, slurplen + 64);
        slurped[slurplen] = '\0';
        slurplen += 64;
        strcat(slurped, buffer);
    }

    return slurped;
}

// Extract a word from a string.
// This returns a heap allocated string; make sure you free it!
char* strword(const char *str, char **end)
{
    char *word = NULL;
    const char *begin = str;

    // Skip whitespace. Coincedentally, isblank checks for space and tab. ;)
    while (isblank(*begin)) {
        begin++;
    }

    *end = (char*) begin; // yes, please discard the const
    while (isgraph(**end)) {
        (*end)++;
    }

    ptrdiff_t diff = *end - begin;
    if (diff < 0) {
        fprintf(stderr, "strtoaw: got invalid pointer difference.");
        abort();
    } else if (diff == 0) {
        return word;
    }

    size_t len = diff + 1;

    // Copy the word.
    word = xmalloc(len);
    strncpy(word, begin, diff);

    word[diff] = '\0';

    return word;
}

char *strslice(const char *str, const char *end)
{
    if (!str || !end) {
        return NULL;
    }
    size_t diff = labs(end - str);
    char *newstr = xmalloc(diff + 1);
    newstr[diff] = '\0';
    memcpy(newstr, str, diff);
    return newstr;
}

int ptrcmp(const void *a, const void *b)
{
	return (const char *)a - (const char *)b;
}

uintptr_t ptrhash(const void *a)
{
	return (uintptr_t) a;
}

void
cact_fatal(const char *fmt, ...)
{
}
