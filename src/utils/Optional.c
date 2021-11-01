#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define TYPE       int
#define NAME       int
#define STRUCTNAME Int
#define IMPLEMENT_OPTIONAL
#include "Optional.h"

TEST(optional_int_new) {
    STARTTEST();

    OptionalInt *optional = optional_int_new();

    TESTASSERT(OptionalInt shouldnt have value, !optional->has_value);

    optional_int_free(optional);

    ENDTEST();
}

TEST(optional_int_set) {
    STARTTEST();

    OptionalInt *optional = optional_int_new();
    int i = 5;
    optional_int_set(optional, &i);

    TESTASSERT(OptionalInt should have a value, optional->has_value);
    TESTASSERT(The value should be 5, *(int *)optional->data == 5);

    optional_int_free(optional);

    ENDTEST();
}

TEST(optional_int_reset_without_value) {
    STARTTEST();

    OptionalInt *optional = optional_int_new();
    optional_int_reset(optional);

    TESTASSERT(OptionalInt still shouldnt have a value, !optional->has_value);

    optional_int_free(optional);

    ENDTEST();
}

TEST(optional_int_reset_with_value) {
    STARTTEST();

    OptionalInt *optional = optional_int_new();
    int i = 5;
    optional_int_set(optional, &i);
    optional_int_reset(optional);

    TESTASSERT(OptionalInt shouldnt have a value anymore, !optional->has_value);

    optional_int_free(optional);

    ENDTEST();
}

TEST(optional_int_has_value) {
    STARTTEST();

    OptionalInt *optional = optional_int_new();

    TESTASSERT(At the beginning it should be false,
               !optional_int_has_value(optional));

    int i = 5;
    optional_int_set(optional, &i);

    TESTASSERT(When set it should be true, optional_int_has_value(optional));

    optional_int_reset(optional);

    TESTASSERT(When reset it should be false,
               !optional_int_has_value(optional));

    optional_int_free(optional);

    ENDTEST();
}

TEST(optional_int_value_or) {
    STARTTEST();

    OptionalInt *optional = optional_int_new();
    int value_or = 5;

    TESTASSERT(Without value it should be the or value,
               *optional_int_value_or(optional, &value_or) == 5);

    int value = 10;
    optional_int_set(optional, &value);

    TESTASSERT(With value it should be that,
               *optional_int_value_or(optional, &value_or) == 10);

    optional_int_free(optional);

    ENDTEST();
}

TEST(optional) {
    STARTTEST();

    RUNTEST(optional_int_new);
    RUNTEST(optional_int_set);
    RUNTEST(optional_int_reset_without_value);
    RUNTEST(optional_int_reset_with_value);
    RUNTEST(optional_int_has_value);
    RUNTEST(optional_int_value_or);

    PRINTTESTSUITE(OptionalInt);

    ENDTEST();
}
