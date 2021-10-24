#pragma once

#include "../tests/framework.h"

#include <stdbool.h>
#include <stddef.h>

#define optional_new(type) _optional_new_base(sizeof(type))

#define optional_value_or(optional, placeholder, type) \
    *(type *)_optional_value_or_base(optional, placeholder)

typedef struct Optional Optional;

Optional *_optional_new_base(size_t data_size);

void optional_free(Optional *optional);

void optional_set(Optional *optional, void *value);

void optional_reset(Optional *optional);

bool optional_has_value(Optional *optional);

void *_optional_value_or_base(Optional *optional, void *placeholder);

TEST(optional);
