#ifndef str_h_INCLUDED
#define str_h_INCLUDED

struct cact_string {
    char *str;
};

DEFINE_OBJECT_CONVERSION(CACT_OBJ_STRING,      struct cact_string, cact_to_string,    str)
DEFINE_OBJECT_CHECK(cact_is_string, CACT_OBJ_STRING)

struct cact_val cact_make_string(struct cactus *, const char *);

#endif // str_h_INCLUDED

