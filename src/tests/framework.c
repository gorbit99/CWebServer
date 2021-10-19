#include "framework.h"

void __print_results(char *test_name, TestResults results) {
    printf("Test %s:\n", test_name);
    printf("\t");
    for (int i = 0; i < results.successful; i++) {
        printf("+");
    }
    for (int i = 0; i < results.failed; i++) {
        printf("-");
    }
    printf("\n\n");
}
