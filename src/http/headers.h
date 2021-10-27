#pragma once

#include <stdint.h>

uint64_t headers_hash(void *key);

int headers_cmp(void *key, void *key2);
