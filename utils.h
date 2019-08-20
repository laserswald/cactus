#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void die(const char* fmt, ...);

/* Read an entire file into a string. */
char *slurp(FILE* f);

/* Get a word from a string, and set end to the rest of the string. */
char *strword(const char* str, char **end);

/* Allocate a copy of a string from beginning to end pointers. */
char *strslice(const char *str, const char *end);

#endif // utils_h_INCLUDED

