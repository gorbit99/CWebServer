#include "tests/framework.h"
#include "utils/Hashmap.h"
#include "utils/Optional.h"
#include "utils/Vector.h"

#include <stdio.h>

TEST(test_suite) {
    STARTTEST();

    RUNTEST(optional);
    RUNTEST(vector);
    RUNTEST(hashmap);

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
