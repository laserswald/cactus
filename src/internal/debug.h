#ifndef __CACTUS_DEBUG_H__
#define __CACTUS_DEBUG_H__

#ifdef NDEBUG
#define DBG(...)
#define DBGVAL(...)
#else
#define DBG(...) fprintf(stdout, __VA_ARGS__)
#define DBGVAL(val, ...) do { \
    DBG(__VA_ARGS__); \
    fprint_sexp(stdout, val); \
    fprintf(stdout, "\n"); \
while (0)
#endif

#endif /* __CACTUS_DEBUG_H__ */

