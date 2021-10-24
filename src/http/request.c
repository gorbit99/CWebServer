#include "request.h"

#include "../utils/Hashmap.h"
#include "socket.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

struct HttpRequest {
    String *verb;
    String *target;
    String *version;
    Hashmap *headers;

    String *body;
};

uint64_t option_hash(void *key) {
    String *str = *(String **)key;

    char *cstr = string_as_cstr(str);

    uint64_t hash = 0;

    uint64_t cur_part = 0;
    int part_counter = 0;
    while (*cstr != '\0') {
        cur_part = cur_part << 8 | *cstr;
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

int option_cmp(void *key1, void *key2) {
    String *str1 = *(String **)key1;
    String *str2 = *(String **)key2;

    return string_strcmp(str1, str2);
}

static char request_option_lowercase(char c) {
    return tolower(c);
}

static void request_headers_print(void *key, void *value) {
    String *name = *(String **)key;
    String *set = *(String **)value;

    printf("[%s]: [%s]\n", string_as_cstr(name), string_as_cstr(set));
}

HttpRequest *request_read(Connection *connection) {
    HttpRequest *result = (HttpRequest *)malloc(sizeof(HttpRequest));

    result->verb = connection_read_word(connection);
    result->target = connection_read_word(connection);
    result->version = connection_read_word(connection);
    result->headers = hashmap_new(String *, String *, option_hash, option_cmp);

    string_free(connection_read_line(connection));

    String *line = connection_read_line(connection);
    while (string_cstrcmp(line, "\r") != 0) {
        string_pop_back(line);

        Vector *parts = string_split(line, ":", false, 2);

        String *key = vector_get(parts, 0, String *);
        String *value = vector_get(parts, 1, String *);

        string_map(key, request_option_lowercase);

        hashmap_insert(result->headers, &key, &value);

        vector_free(parts);
        string_free(line);
        line = connection_read_line(connection);
    }

    string_free(line);

    String *content_type_key = string_from_cstr("content-length");
    Optional *content_type_value =
            hashmap_get(result->headers, &content_type_key);
    string_free(content_type_key);

    if (optional_has_value(content_type_value)) {
        String *content_type =
                optional_value_or(content_type_value, NULL, String *);

        size_t len;
        string_scanf(content_type, "%lu", &len);

        result->body = connection_read_len(connection, len);
    } else {
        result->body = string_new();
    }

    optional_free(content_type_value);

    return result;
}

void request_print(HttpRequest *request) {
    printf("Verb: %s\n", string_as_cstr(request->verb));
    printf("Target: %s\n", string_as_cstr(request->target));
    printf("Version: %s\n", string_as_cstr(request->version));

    hashmap_foreach(request->headers, request_headers_print);

    printf("Body: %s\n", string_as_cstr(request->body));
}

static void request_option_free(void *key, void *value) {
    String *name = *(String **)key;
    String *set = *(String **)value;

    string_free(name);
    string_free(set);
}

void request_free(HttpRequest *request) {
    string_free(request->verb);
    string_free(request->target);
    string_free(request->version);
    hashmap_foreach(request->headers, request_option_free);
    hashmap_free(request->headers);

    string_free(request->body);

    free(request);
}
