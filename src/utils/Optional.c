#include "Optional.h"

#include <memory.h>
#include <stddef.h>
#include <stdlib.h>

struct Optional {
    void *data;
    size_t data_size;
    bool has_value;
};

Optional *optional_new_base(size_t data_size) {
    Optional *result = (Optional *)malloc(data_size);
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

void *optional_value_or(Optional *optional, void *placeholder) {
    return optional->has_value ? optional->data : placeholder;
}
