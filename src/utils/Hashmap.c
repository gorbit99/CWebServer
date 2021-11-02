#include "../tests/framework.h"

#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAPKEYTYPE    int32_t
#define HASHMAPVALUETYPE  int64_t
#define HASHMAPSTRUCTNAME IntInt
#define HASHMAPFUNCNAME   int_int
#define DECLARE_HASHMAP
#define IMPLEMENT_HASHMAP
#include "Hashmap.h"

static uint64_t test_hash_alg(int32_t *key) {
    return (uint64_t)*key;
}

static int test_compare(int32_t *key1, int32_t *key2) {
    int32_t a = *key1;
    int32_t b = *key2;

    return a < b ? -1 : (a > b ? 1 : 0);
}

TEST(hashmap_remove) {
    STARTTEST();

    HashmapIntInt *hashmap =
            hashmap_int_int_new(test_hash_alg, test_compare, NULL, NULL);

    int32_t key = 10;
    int64_t value = 100;
    hashmap_int_int_insert(hashmap, &key, &value);

    int32_t fake_key = 11;

    hashmap_int_int_remove(hashmap, &fake_key);
    TESTASSERT(Nothing should happen with the wrong index,
               hashmap_int_int_contains_key(hashmap, &key));
    TESTASSERT(Nothing should happen with the wrong index,
               !hashmap_int_int_contains_key(hashmap, &fake_key));

    fake_key = 1024 + 10;

    hashmap_int_int_remove(hashmap, &fake_key);
    TESTASSERT(Even if its 1024 apart from the original,
               hashmap_int_int_contains_key(hashmap, &key));
    TESTASSERT(Even if its 1024 apart from the original,
               !hashmap_int_int_contains_key(hashmap, &fake_key));

    hashmap_int_int_remove(hashmap, &key);
    TESTASSERT(But it shouldnt be there after removing it,
               !hashmap_int_int_contains_key(hashmap, &key));

    hashmap_int_int_free(hashmap);

    ENDTEST();
}

TEST(hashmap_get) {
    STARTTEST();

    HashmapIntInt *hashmap =
            hashmap_int_int_new(test_hash_alg, test_compare, NULL, NULL);

    int32_t key = 10;
    int64_t value = 100;
    hashmap_int_int_insert(hashmap, &key, &value);

    int32_t fake_key = 11;

    OptionalValueIntInt *true_result = hashmap_int_int_get(hashmap, &key);
    OptionalValueIntInt *fake_result = hashmap_int_int_get(hashmap, &fake_key);

    TESTASSERT(The wrong key shouldnt have a value,
               !optional_value_int_int_has_value(fake_result));
    TESTASSERT(The right key should have a value,
               optional_value_int_int_has_value(true_result));
    TESTASSERT(And it should be what was put in,
               *(int64_t *)optional_value_int_int_value_or(true_result, 0)
                       == value);

    optional_value_int_int_free(true_result);
    optional_value_int_int_free(fake_result);

    value = 200;
    hashmap_int_int_insert(hashmap, &key, &value);

    OptionalValueIntInt *overwrite_result = hashmap_int_int_get(hashmap, &key);
    TESTASSERT(Overwriting the value should be possible,
               optional_value_int_int_has_value(overwrite_result));
    TESTASSERT(And it should be what it was overwritten by,
               *(int *)optional_value_int_int_value_or(overwrite_result, 0)
                       == value);

    optional_value_int_int_free(overwrite_result);

    hashmap_int_int_free(hashmap);

    ENDTEST();
}

TEST(hashmap_insert_and_contains_key) {
    STARTTEST();

    HashmapIntInt *hashmap =
            hashmap_int_int_new(test_hash_alg, test_compare, NULL, NULL);

    int32_t key = 10;
    int64_t value = 100;
    hashmap_int_int_insert(hashmap, &key, &value);

    TESTASSERT(Hashmap should contain the key,
               hashmap_int_int_contains_key(hashmap, &key));

    int32_t fake_key = 11;

    TESTASSERT(Hashmap shouldnt contain fake key,
               !hashmap_int_int_contains_key(hashmap, &fake_key));

    fake_key = 1024 + 10;

    TESTASSERT(Even if its 1024 apart from the original,
               !hashmap_int_int_contains_key(hashmap, &fake_key));

    hashmap_int_int_free(hashmap);

    ENDTEST();
}

TEST(hashmap) {
    STARTTEST();

    RUNTEST(hashmap_insert_and_contains_key);
    RUNTEST(hashmap_remove);
    RUNTEST(hashmap_get);

    PRINTTESTSUITE(Hashmap);

    ENDTEST();
}
