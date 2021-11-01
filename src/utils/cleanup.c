#include "cleanup.h"

void cleanup_string(void *data) {
    String *string = *(String **)data;
    string_free(string);
}

void cleanup_optional(void *data) {
    Optional *optional = *(Optional **)data;
    optional_free(optional);
}

void cleanup_hashmap(void *data) {
    Hashmap *hashmap = *(Hashmap **)data;
    hashmap_free(hashmap);
}
