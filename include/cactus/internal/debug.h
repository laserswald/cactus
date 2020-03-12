#ifndef debug_h_INCLUDED
#define debug_h_INCLUDED

#ifdef NDEBUG
#define DBG(...)
#define DBGSXP(...)
#else
#define DBG(...) fprintf(stdout, __VA_ARGS__)
#define DBGSXP(...) fprintf(stdout, __VA_ARGS__)
#endif



#endif // debug_h_INCLUDED

