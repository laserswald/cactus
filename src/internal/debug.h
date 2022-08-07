#ifndef __CACTUS_DEBUG_H__
#define __CACTUS_DEBUG_H__

#ifdef CACT_DEBUG

  #define DBG(...) fprintf(stdout, __VA_ARGS__)
  #define DBGVAL(val, ...) do { \
      DBG(__VA_ARGS__); \
      fprint_sexp(stdout, val); \
      fprintf(stdout, "\n"); \
  while (0)

#else

  #define DBG(...)
  #define DBGVAL(...)

#endif

#endif /* __CACTUS_DEBUG_H__ */

