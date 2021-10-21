#pragma once

#include "../tests/framework.h"

#include <stdbool.h>
#include <stddef.h>

#define optional_new(type) _optional_new_base(sizeof(type))

typedef struct Optional Optional;

Optional *_optional_new_base(size_t data_size);

void optional_free(Optional *optional);

void optional_set(Optional *optional, void *value);

void optional_reset(Optional *optional);

bool optional_has_value(Optional *optional);

void *optional_value_or(Optional *optional, void *placeholder);

TEST(optional);
