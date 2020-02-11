#ifndef cact_builtin_h_INCLUDED
#define cact_builtin_h_INCLUDED

typedef struct cact_builtin_table {
    char* name;
    struct cact_val* (*fn)(struct cactus *, struct cact_val *);
} BuiltinTable;

struct cact_val* cact_builtin_if(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_car(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_cdr(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_cons(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_is_nil(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_is_pair(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_is_number(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_is_boolean(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_not(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_eq(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_eqv(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_equal(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_display(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_newline(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_begin(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_exit(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_load(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_plus(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_minus(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_times(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_divide(struct cactus *, struct cact_val *);
struct cact_val* cact_builtin_load(struct cactus *, struct cact_val *);

#endif // cact_builtin_h_INCLUDED

