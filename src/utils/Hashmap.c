#include "Hashmap.h"

#include "../tests/framework.h"
#include "Optional.h"
#include "Vector.h"

#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const size_t HASHMAP_BUCKET_COUNT = 1024;

typedef struct HashmapEntry {
    void *key;
    void *value;
} HashmapEntry;

struct Hashmap {
    Vector *buckets;
    size_t key_size;
    size_t value_size;

    uint64_t (*hash_alg)(void *key);
    int (*compare)(void *key1, void *key2);
    void (*cleanup_key)(void *key);
    void (*cleanup_value)(void *value);
};

Hashmap *_hashmap_new_base(size_t key_size,
                           size_t value_size,
                           uint64_t (*hash_alg)(void *key),
                           int (*compare)(void *key1, void *key2),
                           void (*cleanup_key)(void *key),
                           void (*cleanup_value)(void *value)) {
    Hashmap *result = (Hashmap *)malloc(sizeof(Hashmap));

    result->key_size = key_size;
    result->value_size = value_size;
    result->hash_alg = hash_alg;
    result->compare = compare;
    result->cleanup_key = cleanup_key;
    result->cleanup_value = cleanup_value;

    result->buckets = vector_new(Vector *);
    vector_reserve(result->buckets, HASHMAP_BUCKET_COUNT);

    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        Vector *bucket = vector_new(HashmapEntry);
        vector_push_back(result->buckets, &bucket);
    }

    return result;
}

static Vector *hashmap_get_bucket(Hashmap *hashmap, size_t index) {
    Vector *bucket = vector_get(hashmap->buckets, index, Vector *);

    return bucket;
}

void hashmap_bucket_free(Hashmap *hashmap, Vector *bucket) {
    for (size_t i = 0; i < vector_size(bucket); i++) {
        HashmapEntry entry = vector_get(bucket, i, HashmapEntry);

        if (hashmap->cleanup_key) {
            hashmap->cleanup_key(entry.key);
        }
        if (hashmap->cleanup_value) {
            hashmap->cleanup_value(entry.value);
        }
        free(entry.key);
        free(entry.value);
    }

    vector_free(bucket);
}

void hashmap_free(Hashmap *hashmap) {
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        Vector *bucket = hashmap_get_bucket(hashmap, i);

        hashmap_bucket_free(hashmap, bucket);
    }

    vector_free(hashmap->buckets);

    free(hashmap);
}

static bool hashmap_bucket_bin_search(Hashmap *hashmap,
                                      Vector *bucket,
                                      void *key,
                                      size_t *out_index) {
    if (vector_size(bucket) == 0) {
        return false;
    }

    size_t min_index = 0;
    size_t max_index = vector_size(bucket) - 1;

    while (min_index != max_index) {
        size_t center_index = (min_index + max_index) / 2;

        HashmapEntry entry = vector_get(bucket, center_index, HashmapEntry);

        int compared = hashmap->compare(key, entry.key);

        if (compared < 0) {
            max_index = center_index - 1;
        } else if (compared > 0) {
            min_index = center_index + 1;
        } else {
            min_index = center_index;
            max_index = center_index;
        }
    }

    *out_index = min_index;
    return true;
}

static void hashmap_bucket_insert(Hashmap *hashmap,
                                  Vector *bucket,
                                  void *key,
                                  void *value) {
    size_t index;

    if (!hashmap_bucket_bin_search(hashmap, bucket, key, &index)) {
        HashmapEntry entry;
        entry.key = malloc(hashmap->key_size);
        memcpy(entry.key, key, hashmap->key_size);
        entry.value = malloc(hashmap->value_size);
        memcpy(entry.value, value, hashmap->value_size);

        vector_push_back(bucket, &entry);
        return;
    }

    HashmapEntry entry = vector_get(bucket, index, HashmapEntry);

    int compared = hashmap->compare(key, entry.key);

    if (compared == 0) {
        HashmapEntry entry = vector_get(bucket, index, HashmapEntry);

        if (hashmap->cleanup_value) {
            hashmap->cleanup_value(entry.value);
        }

        memcpy(entry.value, value, hashmap->value_size);
    } else {
        HashmapEntry entry;
        entry.key = malloc(hashmap->key_size);
        memcpy(entry.key, key, hashmap->key_size);
        entry.value = malloc(hashmap->value_size);
        memcpy(entry.value, value, hashmap->value_size);

        vector_insert(bucket, &entry, index + (compared < 0 ? 0 : 1));
    }
}

void _hashmap_insert_base(Hashmap *hashmap, void *key, void *value) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    Vector *bucket = hashmap_get_bucket(hashmap, hash);

    hashmap_bucket_insert(hashmap, bucket, key, value);
}

static void hashmap_bucket_remove(Hashmap *hashmap, Vector *bucket, void *key) {
    size_t index;

    if (!hashmap_bucket_bin_search(hashmap, bucket, key, &index)) {
        return;
    }

    HashmapEntry entry = vector_get(bucket, index, HashmapEntry);

    int compared = hashmap->compare(key, entry.key);

    if (compared == 0) {
        if (hashmap->cleanup_key) {
            hashmap->cleanup_key(entry.key);
        }
        if (hashmap->cleanup_value) {
            hashmap->cleanup_value(entry.value);
        }
        free(entry.key);
        free(entry.value);
        vector_remove_at(bucket, index);
    }
}

void hashmap_remove(Hashmap *hashmap, void *key) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    Vector *bucket = hashmap_get_bucket(hashmap, hash);

    hashmap_bucket_remove(hashmap, bucket, key);
}

static Optional *
        hashmap_bucket_get(Hashmap *hashmap, Vector *bucket, void *key) {
    Optional *result = optional_new(void *);

    size_t index;

    if (!hashmap_bucket_bin_search(hashmap, bucket, key, &index)) {
        return result;
    }

    HashmapEntry entry = vector_get(bucket, index, HashmapEntry);

    int compared = hashmap->compare(key, entry.key);

    if (compared == 0) {
        HashmapEntry entry = vector_get(bucket, index, HashmapEntry);

        optional_set(result, entry.value);
    }

    return result;
}

Optional *hashmap_get(Hashmap *hashmap, void *key) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    Vector *bucket = hashmap_get_bucket(hashmap, hash);

    return hashmap_bucket_get(hashmap, bucket, key);
}

static bool hashmap_bucket_contains_key(Hashmap *hashmap,
                                        Vector *bucket,
                                        void *key) {
    size_t index;

    if (!hashmap_bucket_bin_search(hashmap, bucket, key, &index)) {
        return false;
    }

    HashmapEntry entry = vector_get(bucket, index, HashmapEntry);

    int compared = hashmap->compare(key, entry.key);

    return compared == 0;
}

bool hashmap_contains_key(Hashmap *hashmap, void *key) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    Vector *bucket = hashmap_get_bucket(hashmap, hash);

    return hashmap_bucket_contains_key(hashmap, bucket, key);
}

static uint64_t test_hash_alg(void *key) {
    return (uint64_t) * (int32_t *)key;
}

static int test_compare(void *key1, void *key2) {
    int a = *(int32_t *)key1;
    int b = *(int32_t *)key2;

    return a < b ? -1 : (a > b ? 1 : 0);
}

static void hashmap_bucket_foreach(Vector *bucket,
                                   void (*func)(void *, void *)) {
    size_t size = vector_size(bucket);

    for (size_t i = 0; i < size; i++) {
        HashmapEntry entry = vector_get(bucket, i, HashmapEntry);

        func(entry.key, entry.value);
    }
}

void hashmap_foreach(Hashmap *hashmap, void (*func)(void *, void *)) {
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        Vector *bucket = hashmap_get_bucket(hashmap, i);

        hashmap_bucket_foreach(bucket, func);
    }
}

static void
        hashmap_bucket_foreach_with_data(Vector *bucket,
                                         void (*func)(void *, void *, void *),
                                         void *userdata) {
    size_t size = vector_size(bucket);

    for (size_t i = 0; i < size; i++) {
        HashmapEntry entry = vector_get(bucket, i, HashmapEntry);

        func(entry.key, entry.value, userdata);
    }
}

void hashmap_foreach_with_data(Hashmap *hashmap,
                               void (*func)(void *, void *, void *),
                               void *userdata) {
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        Vector *bucket = hashmap_get_bucket(hashmap, i);

        hashmap_bucket_foreach_with_data(bucket, func, userdata);
    }
}

TEST(hashmap_new) {
    STARTTEST();

    Hashmap *hashmap = hashmap_new(int32_t,
                                   int64_t,
                                   test_hash_alg,
                                   test_compare,
                                   NULL,
                                   NULL);

    TESTASSERT(Key size should be 4, hashmap->key_size == 4);
    TESTASSERT(Value size should be 8, hashmap->value_size == 8);

    hashmap_free(hashmap);

    ENDTEST();
}

TEST(hashmap_remove) {
    STARTTEST();

    Hashmap *hashmap = hashmap_new(int32_t,
                                   int64_t,
                                   test_hash_alg,
                                   test_compare,
                                   NULL,
                                   NULL);

    int32_t key = 10;
    int64_t value = 100;
    _hashmap_insert_base(hashmap, &key, &value);

    int32_t fake_key = 11;

    hashmap_remove(hashmap, &fake_key);
    TESTASSERT(Nothing should happen with the wrong index,
               hashmap_contains_key(hashmap, &key));
    TESTASSERT(Nothing should happen with the wrong index,
               !hashmap_contains_key(hashmap, &fake_key));

    fake_key = 1024 + 10;

    hashmap_remove(hashmap, &fake_key);
    TESTASSERT(Even if its 1024 apart from the original,
               hashmap_contains_key(hashmap, &key));
    TESTASSERT(Even if its 1024 apart from the original,
               !hashmap_contains_key(hashmap, &fake_key));

    hashmap_remove(hashmap, &key);
    TESTASSERT(But it shouldnt be there after removing it,
               !hashmap_contains_key(hashmap, &key));

    hashmap_free(hashmap);

    ENDTEST();
}

TEST(hashmap_get) {
    STARTTEST();

    Hashmap *hashmap = hashmap_new(int32_t,
                                   int64_t,
                                   test_hash_alg,
                                   test_compare,
                                   NULL,
                                   NULL);

    int32_t key = 10;
    int64_t value = 100;
    _hashmap_insert_base(hashmap, &key, &value);

    int32_t fake_key = 11;

    Optional *true_result = hashmap_get(hashmap, &key);
    Optional *fake_result = hashmap_get(hashmap, &fake_key);

    TESTASSERT(The wrong key shouldnt have a value,
               !optional_has_value(fake_result));
    TESTASSERT(The right key should have a value,
               optional_has_value(true_result));
    TESTASSERT(And it should be what was put in,
               optional_value_or(true_result, 0, int64_t) == value);

    optional_free(true_result);
    optional_free(fake_result);

    value = 200;
    _hashmap_insert_base(hashmap, &key, &value);

    Optional *overwrite_result = hashmap_get(hashmap, &key);
    TESTASSERT(Overwriting the value should be possible,
               optional_has_value(overwrite_result));
    TESTASSERT(And it should be what it was overwritten by,
               optional_value_or(overwrite_result, 0, int64_t) == value);

    optional_free(overwrite_result);

    hashmap_free(hashmap);

    ENDTEST();
}

TEST(hashmap_insert_and_contains_key) {
    STARTTEST();

    Hashmap *hashmap = hashmap_new(int32_t,
                                   int64_t,
                                   test_hash_alg,
                                   test_compare,
                                   NULL,
                                   NULL);

    int32_t key = 10;
    int64_t value = 100;
    _hashmap_insert_base(hashmap, &key, &value);

    TESTASSERT(Hashmap should contain the key,
               hashmap_contains_key(hashmap, &key));

    int32_t fake_key = 11;

    TESTASSERT(Hashmap shouldnt contain fake key,
               !hashmap_contains_key(hashmap, &fake_key));

    fake_key = 1024 + 10;

    TESTASSERT(Even if its 1024 apart from the original,
               !hashmap_contains_key(hashmap, &fake_key));

    hashmap_free(hashmap);

    ENDTEST();
}

TEST(hashmap) {
    STARTTEST();

    RUNTEST(hashmap_new);
    RUNTEST(hashmap_insert_and_contains_key);
    RUNTEST(hashmap_remove);
    RUNTEST(hashmap_get);

    PRINTTESTSUITE(Hashmap);

    ENDTEST();
}
