#ifndef CACT_OBJ_H
#define CACT_OBJ_H

enum cact_obj_type {
    CACT_OBJ_PAIR,
    CACT_OBJ_PROCEDURE,
    CACT_OBJ_ENV,
    CACT_LENGTH_OBJ_TYPE,
};

/* A value that has an associated heap component. */
struct cact_obj {
    enum cact_obj_type type;
    union {
        void *ptr;
        struct cact_pair *pair;
        struct cact_proc *proc;
        struct cact_env *env;
    }
}



#endif // CACT_OBJ_H

