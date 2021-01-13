#include <stdio.h>
#include "cactus/read.h"
#include "cactus/val.h"
#include "cactus/builtin.h"
#include "greatest.h"
#include "tests.h"

int tests_run = 0;

SUITE(all_tests) {
    RUN_TEST(boolean_tests);
    RUN_TEST(sexp_tests);
    RUN_TEST(pair_tests);
    RUN_TEST(read_tests);
    RUN_TEST(eval_tests);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(all_tests);
    GREATEST_MAIN_END();
}
