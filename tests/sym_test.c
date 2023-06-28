#include <stdio.h>
#include "greatest.h"

#include "core.h"
#include "evaluator/eval.h"
#include "num.h"
#include "err.h"
#include "write.h"
#include "builtin.h"

// static cact_context_t cact;

SUITE(symbol_tests);

TEST symbol_cmp_test()
{
	PASS();
}

SUITE(symbol_tests)
{
    RUN_TEST(symbol_cmp_test);
}
