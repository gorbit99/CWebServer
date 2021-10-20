#pragma once

#include "Optional.h"

#include <stdbool.h>
#include <stddef.h>

#define hashmap_new(key_type, value_type, hash_alg, compare) \
    hashmap_new_base(sizeof(key_type), sizeof(value_type), hash_alg, compare)

typedef struct Hashmap Hashmap;

Hashmap *hashmap_new_base(size_t key_size, size_t value_size,
                          uint64_t (*hash_alg)(void *key),
                          int (*compare)(void *key1, void *key2));

void hashmap_free(Hashmap *hashmap);

void hashmap_insert(Hashmap *hashmap, void *key, void *value);

void hashmap_remove(Hashmap *hashmap, void *key);

Optional *hashmap_get(Hashmap *hashmap, void *key);

bool hashmap_contains_key(Hashmap *hashmap, void *key);

TEST(hashmap);
