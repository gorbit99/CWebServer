#include "request.h"

#include "../utils/cleanup.h"
#include "headers.h"
#include "socket.h"
#include "string.h"
#include "verb.h"
#include "version.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

struct HttpRequest {
    HttpVerb verb;
    String *target;
    HttpVersion version;
    HashmapStringString *headers;
    HashmapStringString *cookies;

    OptionalString *body;
};

static void request_headers_print(String **key, String **value) {
    String *name = *key;
    String *set = *value;

    printf("[%s]: [%s]\n", string_as_cstr(name), string_as_cstr(set));
}

HttpRequest *request_read(Connection *connection) {
    HttpRequest *result = (HttpRequest *)malloc(sizeof(HttpRequest));

    String *verb_string = connection_read_word(connection);
    result->verb = verb_from_string(verb_string);
    string_free(verb_string);
    result->target = connection_read_word(connection);
    String *version_string = connection_read_word(connection);
    result->headers = hashmap_string_string_new(headers_hash,
                                                headers_cmp,
                                                cleanup_string,
                                                cleanup_string);

    result->cookies = hashmap_string_string_new(string_hash,
                                                string_cmp,
                                                cleanup_string,
                                                cleanup_string);

    result->version = version_from_string(version_string);

    string_free(version_string);

    string_free(connection_read_line(connection));

    String *line = connection_read_line(connection);
    while (string_cstrcmp(line, "\r") != 0) {
        string_pop_back(line);

        VectorString *parts = string_split(line, ":", false, 2);

        if (vector_string_size(parts) != 2) {
            printf("Invalid header format: %s\n", string_as_cstr(line));
            vector_string_foreach(parts, (void (*)(String **))cleanup_string);
            string_free(line);
            continue;
        }

        String *key = *vector_string_get(parts, 0);
        String *value = *vector_string_get(parts, 1);

        string_trim(value);

        hashmap_string_string_insert(result->headers, &key, &value);

        vector_string_free(parts);
        string_free(line);
        line = connection_read_line(connection);
    }

    string_free(line);

    String *content_type_key = string_from_cstr("content-length");
    OptionalValueStringString *content_type_value =
            hashmap_string_string_get(result->headers, &content_type_key);
    string_free(content_type_key);

    if (optional_value_string_string_has_value(content_type_value)) {
        String *content_type =
                *optional_value_string_string_value_or(content_type_value,
                                                       NULL);

        size_t len;
        string_scanf(content_type, "%lu", &len);

        String *body_string = connection_read_len(connection, len);
        result->body = optional_string_new();
        optional_string_set(result->body, &body_string);
    } else {
        result->body = optional_string_new();
    }

    optional_value_string_string_free(content_type_value);

    String *cookie_key = string_from_cstr("Cookie");
    OptionalValueStringString *cookie_header =
            hashmap_string_string_get(result->headers, &cookie_key);
    if (optional_value_string_string_has_value(cookie_header)) {
        String *cookies =
                *optional_value_string_string_value_or(cookie_header, NULL);
        VectorString *cookie_pairs = string_split(cookies, "; ", false, 0);

        for (size_t i = 0; i < vector_string_size(cookie_pairs); i++) {
            String *cookie = *vector_string_get(cookie_pairs, i);
            VectorString *pair = string_split(cookie, "=", false, 2);

            if (vector_string_size(pair) != 2) {
                printf("Invalid cookie format received: %s\n",
                       string_as_cstr(cookie));
                vector_string_foreach(pair,
                                      (void (*)(String **))cleanup_string);
            } else {
                String *key = *vector_string_get(pair, 0);
                String *value = *vector_string_get(pair, 1);

                hashmap_string_string_insert(result->cookies, &key, &value);
            }
            vector_string_free(pair);
        }

        vector_string_foreach(cookie_pairs,
                              (void (*)(String **))cleanup_string);
        vector_string_free(cookie_pairs);
    }
    optional_value_string_string_free(cookie_header);
    string_free(cookie_key);

    connection_discard_data(connection);

    return result;
}

void request_print(HttpRequest *request) {
    String *verb_string = verb_as_string(request->verb);
    printf("Verb: %s\n", string_as_cstr(verb_string));
    string_free(verb_string);
    printf("Target: %s\n", string_as_cstr(request->target));
    String *version_string = version_as_string(request->version);
    printf("Version: %s\n", string_as_cstr(version_string));
    string_free(version_string);

    hashmap_string_string_foreach(request->headers, request_headers_print);

    if (optional_string_has_value(request->body)) {
        printf("Body: %s\n",
               string_as_cstr(*optional_string_value_or(request->body, NULL)));
    }
}

void request_free(HttpRequest *request) {
    string_free(request->target);
    hashmap_string_string_free(request->headers);

    optional_string_map(request->body, cleanup_string);
    optional_string_free(request->body);

    hashmap_string_string_free(request->cookies);

    free(request);
}
