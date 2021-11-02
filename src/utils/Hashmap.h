#include "../tests/framework.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef HASHMAP_TESTS
#define HASHMAP_TESTS
TEST(hashmap);
#endif

#ifdef HASHMAPKEYTYPE

#ifndef HASHMAP_BUCKET_COUNT
#define HASHMAP_BUCKET_COUNT 1024
#endif

#define HASHMAPGLUE(a, b) a##b
#define HASHMAPEVAL(a, b) HASHMAPGLUE(a, b)

#define OPTIONALTYPE       HASHMAPVALUETYPE
#define OPTIONALFUNCNAME   HASHMAPEVAL(value_, HASHMAPFUNCNAME)
#define OPTIONALSTRUCTNAME HASHMAPEVAL(Value, HASHMAPSTRUCTNAME)
#define DECLARE_OPTIONAL
#include "Optional.h"

#define HASHMAPGLUE(a, b)        a##b
#define HASHMAPEVAL(a, b)        HASHMAPGLUE(a, b)
#define HASHMAPGLUE4(a, b, c, d) a##b##c##d
#define HASHMAPEVAL4(a, b, c, d) HASHMAPGLUE4(a, b, c, d)
#define HASHMAPFUNC(action)      HASHMAPEVAL4(hashmap_, HASHMAPFUNCNAME, _, action)
#define HASHMAP                  HASHMAPEVAL(Hashmap, HASHMAPSTRUCTNAME)
#define OPTIONALVALUE            HASHMAPEVAL(OptionalValue, HASHMAPSTRUCTNAME)
#define OPTIONALVALUEFUNC(action) \
    HASHMAPEVAL4(optional_value_, HASHMAPFUNCNAME, _, action)

#ifdef DECLARE_HASHMAP
typedef struct HASHMAP HASHMAP;

HASHMAP *HASHMAPFUNC(new)(uint64_t (*hash_alg)(HASHMAPKEYTYPE *key),
                          int (*compare)(HASHMAPKEYTYPE *key1,
                                         HASHMAPKEYTYPE *key2),
                          void (*cleanup_key)(HASHMAPKEYTYPE *key),
                          void (*cleanup_value)(HASHMAPVALUETYPE *value));

void HASHMAPFUNC(free)(HASHMAP *hashmap);

void HASHMAPFUNC(insert)(HASHMAP *hashmap,
                         HASHMAPKEYTYPE *key,
                         HASHMAPVALUETYPE *value);

void HASHMAPFUNC(remove)(HASHMAP *hashmap, HASHMAPKEYTYPE *key);

OPTIONALVALUE *HASHMAPFUNC(get)(HASHMAP *hashmap, HASHMAPKEYTYPE *key);

bool HASHMAPFUNC(contains_key)(HASHMAP *hashmap, HASHMAPKEYTYPE *key);

void HASHMAPFUNC(foreach)(HASHMAP *hashmap,
                          void (*func)(HASHMAPKEYTYPE *key,
                                       HASHMAPVALUETYPE *value));

void HASHMAPFUNC(foreach_with_data)(HASHMAP *hashmap,
                                    void (*func)(HASHMAPKEYTYPE *key,
                                                 HASHMAPVALUETYPE *value,
                                                 void *userdata),
                                    void *userdata);
#undef DECLARE_HASHMAP
#endif

#ifdef IMPLEMENT_HASHMAP

#define HASHMAPENTRY HASHMAPEVAL(HashmapEntry, HASHMAPSTRUCTNAME)

typedef struct HASHMAPENTRY {
    HASHMAPKEYTYPE *key;
    HASHMAPVALUETYPE *value;
} HASHMAPENTRY;

#define VECTORTYPE       HASHMAPENTRY
#define VECTORFUNCNAME   HASHMAPEVAL(bucket_, HASHMAPFUNCNAME)
#define VECTORSTRUCTNAME HASHMAPEVAL(Bucket, HASHMAPSTRUCTNAME)
#define DECLARE_VECTOR
#define IMPLEMENT_VECTOR
#include "Vector.h"

#define BUCKET HASHMAPEVAL(VectorBucket, HASHMAPSTRUCTNAME)
#define BUCKETFUNC(action) \
    HASHMAPEVAL4(vector_bucket_, HASHMAPFUNCNAME, _, action)

#define VECTORTYPE       BUCKET *
#define VECTORFUNCNAME   HASHMAPEVAL(hashmap_data_, HASHMAPFUNCNAME)
#define VECTORSTRUCTNAME HASHMAPEVAL(HashmapData, HASHMAPSTRUCTNAME)
#define DECLARE_VECTOR
#define IMPLEMENT_VECTOR
#include "Vector.h"

#define HASHMAPDATA HASHMAPEVAL(VectorHashmapData, HASHMAPSTRUCTNAME)
#define HASHMAPDATAFUNC(action) \
    HASHMAPEVAL4(vector_hashmap_data_, HASHMAPFUNCNAME, _, action)

#define OPTIONALTYPE       HASHMAPVALUETYPE
#define OPTIONALFUNCNAME   HASHMAPEVAL(value_, HASHMAPFUNCNAME)
#define OPTIONALSTRUCTNAME HASHMAPEVAL(Value, HASHMAPSTRUCTNAME)
#define IMPLEMENT_OPTIONAL
#include "Optional.h"

typedef struct HASHMAP {
    HASHMAPDATA *buckets;

    uint64_t (*hash_alg)(HASHMAPKEYTYPE *);
    int (*compare)(HASHMAPKEYTYPE *, HASHMAPKEYTYPE *);
    void (*cleanup_key)(HASHMAPKEYTYPE *);
    void (*cleanup_value)(HASHMAPVALUETYPE *);
} HASHMAP;

HASHMAP *HASHMAPFUNC(new)(uint64_t (*hash_alg)(HASHMAPKEYTYPE *key),
                          int (*compare)(HASHMAPKEYTYPE *key1,
                                         HASHMAPKEYTYPE *key2),
                          void (*cleanup_key)(HASHMAPKEYTYPE *key),
                          void (*cleanup_value)(HASHMAPVALUETYPE *value)) {
    HASHMAP *result = (HASHMAP *)malloc(sizeof(HASHMAP));
    result->buckets = HASHMAPDATAFUNC(new)();
    result->hash_alg = hash_alg;
    result->compare = compare;
    result->cleanup_key = cleanup_key;
    result->cleanup_value = cleanup_value;

    HASHMAPDATAFUNC(reserve)(result->buckets, HASHMAP_BUCKET_COUNT);

    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        BUCKET *bucket = BUCKETFUNC(new)();
        HASHMAPDATAFUNC(push_back)(result->buckets, &bucket);
    }

    return result;
}

static void HASHMAPFUNC(bucket_free)(HASHMAP *hashmap, BUCKET *bucket) {
    for (size_t i = 0; i < BUCKETFUNC(size)(bucket); i++) {
        HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, i);

        if (hashmap->cleanup_key) {
            hashmap->cleanup_key(entry->key);
        }
        if (hashmap->cleanup_value) {
            hashmap->cleanup_value(entry->value);
        }
        free(entry->key);
        free(entry->value);
    }

    BUCKETFUNC(free)(bucket);
}

void HASHMAPFUNC(free)(HASHMAP *hashmap) {
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, i);

        HASHMAPFUNC(bucket_free)(hashmap, bucket);
    }

    HASHMAPDATAFUNC(free)(hashmap->buckets);

    free(hashmap);
}

static bool HASHMAPFUNC(bucket_bin_search)(HASHMAP *hashmap,
                                           BUCKET *bucket,
                                           void *key,
                                           size_t *out_index) {
    if (BUCKETFUNC(size)(bucket) == 0) {
        return false;
    }

    size_t min_index = 0;
    size_t max_index = BUCKETFUNC(size)(bucket) - 1;

    while (min_index != max_index) {
        size_t center_index = (min_index + max_index) / 2;

        HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, center_index);

        int compared = hashmap->compare(key, entry->key);

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

static void HASHMAPFUNC(bucket_insert)(HASHMAP *hashmap,
                                       BUCKET *bucket,
                                       HASHMAPKEYTYPE *key,
                                       HASHMAPVALUETYPE *value) {
    size_t index;

    if (!HASHMAPFUNC(bucket_bin_search)(hashmap, bucket, key, &index)) {
        HASHMAPENTRY entry;
        entry.key = (HASHMAPKEYTYPE *)malloc(sizeof(HASHMAPKEYTYPE));
        *entry.key = *key;
        entry.value = (HASHMAPVALUETYPE *)malloc(sizeof(HASHMAPVALUETYPE));
        *entry.value = *value;

        BUCKETFUNC(push_back)(bucket, &entry);
        return;
    }

    HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, index);

    int compared = hashmap->compare(key, entry->key);

    if (compared == 0) {
        if (hashmap->cleanup_value) {
            hashmap->cleanup_value(entry->value);
        }

        *entry->value = *value;
    } else {
        HASHMAPENTRY entry;
        entry.key = (HASHMAPKEYTYPE *)malloc(sizeof(HASHMAPKEYTYPE));
        *entry.key = *key;
        entry.value = (HASHMAPVALUETYPE *)malloc(sizeof(HASHMAPVALUETYPE));
        *entry.value = *value;

        BUCKETFUNC(insert)(bucket, &entry, index + (compared < 0 ? 0 : 1));
    }
}

void HASHMAPFUNC(insert)(HASHMAP *hashmap,
                         HASHMAPKEYTYPE *key,
                         HASHMAPVALUETYPE *value) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, hash);

    HASHMAPFUNC(bucket_insert)(hashmap, bucket, key, value);
}

static void HASHMAPFUNC(bucket_remove)(HASHMAP *hashmap,
                                       BUCKET *bucket,
                                       void *key) {
    size_t index;

    if (!HASHMAPFUNC(bucket_bin_search)(hashmap, bucket, key, &index)) {
        return;
    }

    HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, index);

    int compared = hashmap->compare(key, entry->key);

    if (compared == 0) {
        if (hashmap->cleanup_key) {
            hashmap->cleanup_key(entry->key);
        }
        if (hashmap->cleanup_value) {
            hashmap->cleanup_value(entry->value);
        }
        free(entry->key);
        free(entry->value);
        BUCKETFUNC(remove_at)(bucket, index);
    }
}

void HASHMAPFUNC(remove)(HASHMAP *hashmap, HASHMAPKEYTYPE *key) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, hash);

    HASHMAPFUNC(bucket_remove)(hashmap, bucket, key);
}

static OPTIONALVALUE *
        HASHMAPFUNC(bucket_get)(HASHMAP *hashmap, BUCKET *bucket, void *key) {
    OPTIONALVALUE *result = OPTIONALVALUEFUNC(new)();

    size_t index;

    if (!HASHMAPFUNC(bucket_bin_search)(hashmap, bucket, key, &index)) {
        return result;
    }

    HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, index);

    int compared = hashmap->compare(key, entry->key);

    if (compared == 0) {
        OPTIONALVALUEFUNC(set)(result, entry->value);
    }

    return result;
}

OPTIONALVALUE *HASHMAPFUNC(get)(HASHMAP *hashmap, HASHMAPKEYTYPE *key) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, hash);

    return HASHMAPFUNC(bucket_get)(hashmap, bucket, key);
}

static bool HASHMAPFUNC(bucket_contains_key)(HASHMAP *hashmap,
                                             BUCKET *bucket,
                                             void *key) {
    size_t index;

    if (!HASHMAPFUNC(bucket_bin_search)(hashmap, bucket, key, &index)) {
        return false;
    }

    HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, index);

    int compared = hashmap->compare(key, entry->key);

    return compared == 0;
}

bool HASHMAPFUNC(contains_key)(HASHMAP *hashmap, HASHMAPKEYTYPE *key) {
    uint64_t hash = hashmap->hash_alg(key);

    hash %= HASHMAP_BUCKET_COUNT;

    BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, hash);

    return HASHMAPFUNC(bucket_contains_key)(hashmap, bucket, key);
}

static void HASHMAPFUNC(bucket_foreach)(BUCKET *bucket,
                                        void (*func)(HASHMAPKEYTYPE *,
                                                     HASHMAPVALUETYPE *)) {
    size_t size = BUCKETFUNC(size)(bucket);

    for (size_t i = 0; i < size; i++) {
        HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, i);

        func(entry->key, entry->value);
    }
}

void HASHMAPFUNC(foreach)(HASHMAP *hashmap,
                          void (*func)(HASHMAPKEYTYPE *key,
                                       HASHMAPVALUETYPE *value)) {
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, i);

        HASHMAPFUNC(bucket_foreach)(bucket, func);
    }
}

static void HASHMAPFUNC(bucket_foreach_with_data)(
        BUCKET *bucket,
        void (*func)(HASHMAPKEYTYPE *, HASHMAPVALUETYPE *, void *),
        void *userdata) {
    size_t size = BUCKETFUNC(size)(bucket);

    for (size_t i = 0; i < size; i++) {
        HASHMAPENTRY *entry = BUCKETFUNC(get)(bucket, i);

        func(entry->key, entry->value, userdata);
    }
}

void HASHMAPFUNC(foreach_with_data)(HASHMAP *hashmap,
                                    void (*func)(HASHMAPKEYTYPE *key,
                                                 HASHMAPVALUETYPE *value,
                                                 void *userdata),
                                    void *userdata) {
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++) {
        BUCKET *bucket = *HASHMAPDATAFUNC(get)(hashmap->buckets, i);

        HASHMAPFUNC(bucket_foreach_with_data)(bucket, func, userdata);
    }
}
#undef IMPLEMENT_HASHMAP
#endif

#undef HASHMAPGLUE
#undef HASHMAPEVAL
#undef HASHMAPGLUE4
#undef HASHMAPEVAL4
#undef HASHMAPFUNC
#undef HASHMAP
#undef HASHMAPKEYTYPE
#undef HASHMAPVALUETYPE
#undef HASHMAPFUNCNAME
#undef HASHMAPSTRUCTNAME
#endif // HASHMAPKEYTYPE
