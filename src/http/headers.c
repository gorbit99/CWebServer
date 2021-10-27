#include "headers.h"

#include "../utils/string.h"

#include <ctype.h>

uint64_t headers_hash(void *key) {
    String *str = *(String **)key;

    char *cstr = string_as_cstr(str);

    uint64_t hash = 0;

    uint64_t cur_part = 0;
    int part_counter = 0;
    while (*cstr != '\0') {
        uint64_t converted_part = (uint64_t)tolower(*cstr) & 0xff;
        cur_part = cur_part << 8 | converted_part;
        part_counter++;

        if (part_counter == 4) {
            hash ^= cur_part;
            cur_part = 0;
            part_counter = 0;
        }
        cstr++;
    }

    hash ^= cur_part;

    return hash;
}

int headers_cmp(void *key1, void *key2) {
    String *str1 = *(String **)key1;
    String *str2 = *(String **)key2;

    return string_strcmp_case_insensitive(str1, str2);
}
