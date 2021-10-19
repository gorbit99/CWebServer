#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    int32_t successful;
    int32_t failed;
} TestResults;

#define TEST(name) TestResults test_##name(void)

#define STARTTEST() TestResults test_results = {.successful = 0, .failed = 0}

#define RUNTEST(name) \
    { \
        TestResults newResults = test_##name(); \
        test_results.successful += newResults.successful; \
        test_results.failed += newResults.failed; \
    }

#define PRINTTESTSUITE(name) __print_results(#name, test_results)

#define ENDTEST() return test_results

#define TESTASSERT(name, to_check) \
    if (to_check) { \
        test_results.successful++; \
    } else { \
        printf("Failed test \"%s\"\n", #name); \
        test_results.failed++; \
    }

void __print_results(char *test_name, TestResults results);
