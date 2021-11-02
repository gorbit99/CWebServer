#pragma once

#include "../utils/string.h"

#include <stdint.h>

#define HASHMAPKEYTYPE    String *
#define HASHMAPVALUETYPE  String *
#define HASHMAPFUNCNAME   string_string
#define HASHMAPSTRUCTNAME StringString
#define DECLARE_HASHMAP
#include "../utils/Hashmap.h"

uint64_t headers_hash(String **key);

int headers_cmp(String **key, String **key2);
