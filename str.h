#ifndef str_h_INCLUDED
#define str_h_INCLUDED

typedef struct cact_string {
    char *str;
} cact_string;

struct cact_val cact_make_string(char *str);

#endif // str_h_INCLUDED

