#include <stdio.h>
#include "greatest.h"

#include "core.h"
#include "evaluator/eval.h"
#include "num.h"
#include "err.h"
#include "write.h"
#include "builtin.h"

static struct cactus cact;

SUITE(symbol_tests);

TEST symbol_cmp_test()
{
	char *name = "simplesym"
	PASS();
}

SUITE(symbol_tests)
{
    RUN_TEST(symbol_cmp_test);
}
