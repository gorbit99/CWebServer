#include "tests/framework.h"
#include "utils/Optional.h"

#include <stdio.h>

TEST(test_suite) {
    STARTTEST();

    RUNTEST(optional_tests);

    ENDTEST();
}

int main(void) {
#ifdef TESTING
    test_test_suite();
#else
    printf("Hello World\n");
    return 0;
#endif
}
