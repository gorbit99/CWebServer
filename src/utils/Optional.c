#include "Optional.h"

#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct Optional {
    void *data;
    size_t data_size;
    bool has_value;
};

Optional *_optional_new_base(size_t data_size) {
    Optional *result = (Optional *)malloc(sizeof(Optional));
    result->data = malloc(sizeof(data_size));
    result->data_size = data_size;
    result->has_value = false;

    return result;
}

void optional_free(Optional *optional) {
    free(optional->data);
    free(optional);
}

void optional_set(Optional *optional, void *value) {
    memcpy(optional->data, value, optional->data_size);
    optional->has_value = true;
}

void optional_reset(Optional *optional) {
    optional->has_value = false;
}

bool optional_has_value(Optional *optional) {
    return optional->has_value;
}

void *_optional_value_or_base(Optional *optional, void *placeholder) {
    return optional->has_value ? optional->data : placeholder;
}

TEST(optional_new) {
    STARTTEST();

    Optional *optional = optional_new(int32_t);

    TESTASSERT(Data size should be 4, optional->data_size == 4);
    TESTASSERT(Optional shouldnt have value, !optional->has_value);

    optional_free(optional);

    ENDTEST();
}

TEST(optional_set) {
    STARTTEST();

    Optional *optional = optional_new(int32_t);
    int i = 5;
    optional_set(optional, &i);

    TESTASSERT(Optional should have a value, optional->has_value);
    TESTASSERT(The value should be 5, *(int *)optional->data == 5);

    optional_free(optional);

    ENDTEST();
}

TEST(optional_reset_without_value) {
    STARTTEST();

    Optional *optional = optional_new(int32_t);
    optional_reset(optional);

    TESTASSERT(Optional still shouldnt have a value, !optional->has_value);

    optional_free(optional);

    ENDTEST();
}

TEST(optional_reset_with_value) {
    STARTTEST();

    Optional *optional = optional_new(int32_t);
    int i = 5;
    optional_set(optional, &i);
    optional_reset(optional);

    TESTASSERT(Optional shouldnt have a value anymore, !optional->has_value);

    optional_free(optional);

    ENDTEST();
}

TEST(optional_has_value) {
    STARTTEST();

    Optional *optional = optional_new(int32_t);

    TESTASSERT(At the beginning it should be false,
               !optional_has_value(optional));

    int i = 5;
    optional_set(optional, &i);

    TESTASSERT(When set it should be true, optional_has_value(optional));

    optional_reset(optional);

    TESTASSERT(When reset it should be false, !optional_has_value(optional));

    optional_free(optional);

    ENDTEST();
}

TEST(optional_value_or) {
    STARTTEST();

    Optional *optional = optional_new(int32_t);
    int value_or = 5;

    TESTASSERT(Without value it should be the or value,
               *(int *)_optional_value_or_base(optional, &value_or) == 5);

    int value = 10;
    optional_set(optional, &value);

    TESTASSERT(With value it should be that,
               *(int *)_optional_value_or_base(optional, &value_or) == 10);

    optional_free(optional);

    ENDTEST();
}

TEST(optional) {
    STARTTEST();

    RUNTEST(optional_new);
    RUNTEST(optional_set);
    RUNTEST(optional_reset_without_value);
    RUNTEST(optional_reset_with_value);
    RUNTEST(optional_has_value);
    RUNTEST(optional_value_or);

    PRINTTESTSUITE(Optional);

    ENDTEST();
}
