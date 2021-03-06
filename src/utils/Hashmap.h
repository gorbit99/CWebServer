#pragma once

#include "Optional.h"

#include <stdbool.h>
#include <stddef.h>

#define hashmap_new(key_type, \
                    value_type, \
                    hash_alg, \
                    compare, \
                    cleanup_key, \
                    cleanup_value) \
    _hashmap_new_base(sizeof(key_type), \
                      sizeof(value_type), \
                      hash_alg, \
                      compare, \
                      cleanup_key, \
                      cleanup_value)

#define hashmap_insert(hashmap, key, value) \
    _hashmap_insert_base(hashmap, &key, &value)

typedef struct Hashmap Hashmap;

Hashmap *_hashmap_new_base(size_t key_size,
                           size_t value_size,
                           uint64_t (*hash_alg)(void *key),
                           int (*compare)(void *key1, void *key2),
                           void (*cleanup_key)(void *key),
                           void (*cleanup_value)(void *value));

void hashmap_free(Hashmap *hashmap);

void _hashmap_insert_base(Hashmap *hashmap, void *key, void *value);

void hashmap_remove(Hashmap *hashmap, void *key);

Optional *hashmap_get(Hashmap *hashmap, void *key);

bool hashmap_contains_key(Hashmap *hashmap, void *key);

void hashmap_foreach(Hashmap *hashmap, void (*func)(void *key, void *value));

void hashmap_foreach_with_data(Hashmap *hashmap,
                               void (*func)(void *key,
                                            void *value,
                                            void *userdata),
                               void *userdata);

TEST(hashmap);
