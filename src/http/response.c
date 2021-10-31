#include "response.h"

#include "../utils/Hashmap.h"
#include "../utils/Vector.h"
#include "../utils/string.h"
#include "cookie.h"
#include "headers.h"
#include "response_codes.h"
#include "socket.h"
#include "version.h"

#include <stdlib.h>
#include <time.h>

struct HttpResponse {
    HttpVersion version;
    HttpResponseCode response_code;
    String *status;
    Hashmap *headers;

    Optional *body;
    Vector *set_cookies;
};

static void response_print_header(void *key, void *value, void *userdata) {
    Connection *connection = (Connection *)userdata;
    String *name_string = *(String **)key;
    String *value_string = *(String **)value;

    connection_printf(connection,
                      "%s: %s\n",
                      string_as_cstr(name_string),
                      string_as_cstr(value_string));
}

static void response_print_set_cookie(void *data, void *userdata) {
    Connection *connection = (Connection *)userdata;
    CookieRequest *cookie_request = *(CookieRequest **)data;

    cookie_print_to_connection(cookie_request, connection);
}

void response_send_to_connection(HttpResponse *response,
                                 Connection *connection) {
    String *version_string = version_as_string(response->version);
    connection_printf(connection,
                      "%s %u %s\n",
                      string_as_cstr(version_string),
                      response->response_code,
                      string_as_cstr(response->status));
    string_free(version_string);
    hashmap_foreach_with_data(response->headers,
                              response_print_header,
                              connection);
    vector_foreach_with_data(response->set_cookies,
                             response_print_set_cookie,
                             connection);

    if (optional_has_value(response->body)) {
        String *body_string = optional_value_or(response->body, NULL, String *);
        connection_printf(connection, "\n%s\n", string_as_cstr(body_string));
    }
}

void response_free_cookie(void *data) {
    CookieRequest *cookie_request = *(CookieRequest **)data;
    cookie_request_free(cookie_request);
}

void response_free(HttpResponse *response) {
    string_free(response->status);
    hashmap_free(response->headers);

    if (optional_has_value(response->body)) {
        String *value = optional_value_or(response->body, NULL, String *);

        string_free(value);
    }

    vector_foreach(response->set_cookies, response_free_cookie);
    vector_free(response->set_cookies);

    free(response);
}

struct HttpResponseBuilder {
    HttpResponse *response;
};

HttpResponse *response_builder_build(HttpResponseBuilder *response_builder) {
    HttpResponse *result = response_builder->response;
    free(response_builder);

    return result;
}

static void response_cleanup_header(void *data) {
    String *data_string = *(String **)data;

    string_free(data_string);
}

static void response_builder_add_header(HttpResponseBuilder *response_builder,
                                        char *name,
                                        char *value) {
    String *date_key = string_from_cstr(name);
    String *date_string = string_from_cstr(value);

    hashmap_insert(response_builder->response->headers, date_key, date_string);
}

HttpResponseBuilder *response_builder_new(void) {
    HttpResponseBuilder *result =
            (HttpResponseBuilder *)malloc(sizeof(HttpResponseBuilder));

    result->response = (HttpResponse *)malloc(sizeof(HttpResponse));

    result->response->response_code = HttpOk;
    result->response->version = Http11;
    result->response->body = optional_new(String *);
    result->response->headers = hashmap_new(String *,
                                            String *,
                                            headers_hash,
                                            headers_cmp,
                                            response_cleanup_header,
                                            response_cleanup_header);
    result->response->status = response_code_as_string(HttpOk);

    time_t raw_time;
    time(&raw_time);

    struct tm *time_info = gmtime(&raw_time);

    char datetime[100];
    strftime(datetime, 100, "%a, %d %b %Y %H:%M:%S %Z", time_info);

    response_builder_add_header(result, "Date", datetime);
    response_builder_add_header(result, "Content-Type", "text/html");
    response_builder_add_header(result, "Server", "GorbitsWebServer");

    result->response->set_cookies = vector_new(CookieRequest *);

    return result;
}

void response_builder_set_version(HttpResponseBuilder *response_builder,
                                  HttpVersion version) {
    response_builder->response->version = version;
}

void response_builder_set_code(HttpResponseBuilder *response_builder,
                               HttpResponseCode code,
                               bool set_status) {
    response_builder->response->response_code = code;

    if (set_status) {
        response_builder_set_status(response_builder,
                                    response_code_as_string(code));
    }
}

void response_builder_set_header(HttpResponseBuilder *response_builder,
                                 String *name,
                                 String *value) {
    hashmap_insert(response_builder->response->headers, name, value);
}

void response_builder_set_body(HttpResponseBuilder *response_builder,
                               String *body) {
    if (optional_has_value(response_builder->response->body)) {
        String *value = optional_value_or(response_builder->response->body,
                                          NULL,
                                          String *);

        string_free(value);
    }
    optional_set(response_builder->response->body, body);

    String *content_length_key = string_from_cstr("Content-Length");
    size_t body_size = string_size(body);
    char content_length[30];
    sprintf(content_length, "%lu", body_size);
    String *content_length_string = string_from_cstr(content_length);

    hashmap_insert(response_builder->response->headers,
                   content_length_key,
                   content_length_string);
}

void response_builder_remove_body(HttpResponseBuilder *response_builder) {
    optional_reset(response_builder->response->body);

    String *content_length_key = string_from_cstr("Content-Length");

    hashmap_remove(response_builder->response->headers, content_length_key);

    string_free(content_length_key);
}

void response_builder_set_status(HttpResponseBuilder *response_builder,
                                 String *status) {
    string_free(response_builder->response->status);
    response_builder->response->status = status;
}

void response_builder_add_set_cookie(HttpResponseBuilder *response_builder,
                                     CookieRequest *cookie_request) {
    vector_push_back(response_builder->response->set_cookies, &cookie_request);
}
