#include "request.h"

#include "../utils/Hashmap.h"
#include "../utils/cleanup.h"
#include "headers.h"
#include "socket.h"
#include "verb.h"
#include "version.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

struct HttpRequest {
    HttpVerb verb;
    String *target;
    HttpVersion version;
    Hashmap *headers;
    Hashmap *cookies;

    Optional *body;
};

static void request_headers_print(void *key, void *value) {
    String *name = *(String **)key;
    String *set = *(String **)value;

    printf("[%s]: [%s]\n", string_as_cstr(name), string_as_cstr(set));
}

HttpRequest *request_read(Connection *connection) {
    HttpRequest *result = (HttpRequest *)malloc(sizeof(HttpRequest));

    String *verb_string = connection_read_word(connection);
    result->verb = verb_from_string(verb_string);
    string_free(verb_string);
    result->target = connection_read_word(connection);
    String *version_string = connection_read_word(connection);
    result->headers = hashmap_new(String *,
                                  String *,
                                  headers_hash,
                                  headers_cmp,
                                  cleanup_string,
                                  cleanup_string);

    result->cookies = hashmap_new(String *,
                                  String *,
                                  string_hash,
                                  string_cmp,
                                  cleanup_string,
                                  cleanup_string);

    result->version = version_from_string(version_string);

    string_free(version_string);

    string_free(connection_read_line(connection));

    String *line = connection_read_line(connection);
    while (string_cstrcmp(line, "\r") != 0) {
        string_pop_back(line);

        Vector *parts = string_split(line, ":", false, 2);

        if (vector_size(parts) != 2) {
            printf("Invalid header format: %s\n", string_as_cstr(line));
            vector_foreach(parts, cleanup_string);
            string_free(line);
            continue;
        }

        String *key = vector_get(parts, 0, String *);
        String *value = vector_get(parts, 1, String *);

        string_trim(value);

        hashmap_insert(result->headers, key, value);

        vector_free(parts);
        string_free(line);
        line = connection_read_line(connection);
    }

    string_free(line);

    String *content_type_key = string_from_cstr("content-length");
    Optional *content_type_value =
            hashmap_get(result->headers, content_type_key);
    string_free(content_type_key);

    if (optional_has_value(content_type_value)) {
        String *content_type =
                optional_value_or(content_type_value, NULL, String *);

        size_t len;
        string_scanf(content_type, "%lu", &len);

        String *body_string = connection_read_len(connection, len);
        result->body = optional_new(String *);
        optional_set(result->body, body_string);
    } else {
        result->body = optional_new(String *);
    }

    optional_free(content_type_value);

    String *cookie_key = string_from_cstr("Cookie");
    Optional *cookie_header = hashmap_get(result->headers, cookie_key);
    if (optional_has_value(cookie_header)) {
        String *cookies = optional_value_or(cookie_header, NULL, String *);
        Vector *cookie_pairs = string_split(cookies, "; ", false, 0);

        for (size_t i = 0; i < vector_size(cookie_pairs); i++) {
            String *cookie = vector_get(cookie_pairs, i, String *);
            Vector *pair = string_split(cookie, "=", false, 2);

            if (vector_size(pair) != 2) {
                printf("Invalid cookie format received: %s\n",
                       string_as_cstr(cookie));
                vector_foreach(pair, cleanup_string);
            } else {
                String *key = vector_get(pair, 0, String *);
                String *value = vector_get(pair, 1, String *);

                hashmap_insert(result->cookies, key, value);
            }
            vector_free(pair);
        }

        vector_foreach(cookie_pairs, cleanup_string);
        vector_free(cookie_pairs);
    }
    optional_free(cookie_header);
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

    hashmap_foreach(request->headers, request_headers_print);

    if (optional_has_value(request->body)) {
        printf("Body: %s\n",
               string_as_cstr(
                       optional_value_or(request->body, NULL, String *)));
    }
}

void request_free(HttpRequest *request) {
    string_free(request->target);
    hashmap_free(request->headers);

    optional_map(request->body, string_free);
    optional_free(request->body);

    hashmap_free(request->cookies);

    free(request);
}
