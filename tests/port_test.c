#include <stdio.h>
#include "greatest.h"

#include "core.h"
#include "read.h"
#include "val.h"
#include "port.h"
#include "internal/utils.h"

static struct cactus cact;

SUITE(port_tests);

TEST is_port_test() {
	struct cact_val iport = cact_make_input_port(&cact, "tests/port_test_file.txt");
	ASSERT(cact_is_port(iport));
	ASSERT(cact_is_input_port(iport));
	ASSERT(!cact_is_output_port(iport));

	struct cact_val oport = cact_make_output_port(&cact, "tests/port_test_file.txt");
	ASSERT(cact_is_port(oport));
	ASSERT(cact_is_output_port(oport));
	ASSERT(! cact_is_input_port(oport));
	return 0;
}

SUITE(port_tests)
{
    cact_init(&cact);
    RUN_TEST(is_port_test);
    cact_finish(&cact);
}

