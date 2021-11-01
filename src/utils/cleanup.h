#pragma once

#include "Hashmap.h"
#include "Optional.h"
#include "Vector.h"
#include "string.h"

void cleanup_string(void *data);

void cleanup_optional(void *data);

void cleanup_vector(void *data);

void cleanup_hashmap(void *data);
