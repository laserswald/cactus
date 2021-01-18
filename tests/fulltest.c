#include <stdio.h>
#include "cactus/read.h"
#include "cactus/val.h"
#include "cactus/builtin.h"
#include "greatest.h"

SUITE_EXTERN(boolean_tests);
SUITE_EXTERN(sexp_tests);
SUITE_EXTERN(pair_tests);
SUITE_EXTERN(read_tests);
SUITE_EXTERN(eval_tests);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {

    GREATEST_MAIN_BEGIN();

    // Core interpreter tests.
    RUN_SUITE(read_tests);
    RUN_SUITE(eval_tests);

    // Value type tests.
    RUN_SUITE(sexp_tests);
    RUN_SUITE(boolean_tests);

    // Object type tests.
    RUN_SUITE(pair_tests);

    GREATEST_MAIN_END();
}
