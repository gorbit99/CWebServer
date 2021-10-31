#include "cookie.h"

#include "../utils/Errors.h"
#include "../utils/Optional.h"
#include "../utils/string.h"
#include "socket.h"

#include <stdlib.h>

struct CookieRequest {
    String *name;
    String *value;
    Optional *expires;
    Optional *max_age;
    bool secure;
    bool http_only;
    Optional *domain;
    Optional *path;
    HttpSameSite same_site;
};

static void cookie_free_optional_string(void *data) {
    String *string = *(String **)data;
    string_free(string);
}

void cookie_request_free(CookieRequest *cookie_request) {
    string_free(cookie_request->name);
    string_free(cookie_request->value);
    optional_free(cookie_request->expires);
    optional_free(cookie_request->max_age);
    optional_map(cookie_request->domain, cookie_free_optional_string);
    optional_free(cookie_request->domain);
    optional_map(cookie_request->path, cookie_free_optional_string);
    optional_free(cookie_request->path);
    free(cookie_request);
}

static String *time_to_date(time_t time) {
    struct tm *time_info = gmtime(&time);

    char datetime[100];
    strftime(datetime, 100, "%a, %d %b %Y %H:%M:%S %Z", time_info);
    return string_from_cstr(datetime);
}

void cookie_print_to_connection(CookieRequest *cookie_request,
                                Connection *connection) {
    connection_printf(connection,
                      "Set-Cookie: %s=%s",
                      string_as_cstr(cookie_request->name),
                      string_as_cstr(cookie_request->value));
    if (optional_has_value(cookie_request->expires)) {
        time_t expires = optional_value_or(cookie_request->expires, 0, time_t);

        String *expires_string = time_to_date(expires);
        connection_printf(connection,
                          "; Expires=%s",
                          string_as_cstr(expires_string));
        string_free(expires_string);
    }

    if (optional_has_value(cookie_request->max_age)) {
        time_t max_age = optional_value_or(cookie_request->max_age, 0, time_t);

        connection_printf(connection, "; Max-Age=%ld", max_age);
    }

    if (cookie_request->secure) {
        connection_printf(connection, "; Secure");
    }

    if (cookie_request->http_only) {
        connection_printf(connection, "; HttpOnly");
    }

    if (optional_has_value(cookie_request->domain)) {
        String *domain =
                optional_value_or(cookie_request->domain, NULL, String *);
        connection_printf(connection, "; Domain=%s", string_as_cstr(domain));
    }

    if (optional_has_value(cookie_request->path)) {
        String *path = optional_value_or(cookie_request->path, NULL, String *);
        connection_printf(connection, "; Path=%s", string_as_cstr(path));
    }

    switch (cookie_request->same_site) {
    case HttpNone:
        connection_printf(connection, "; Same-Site=None");
        break;
    case HttpLax:
        connection_printf(connection, "; Same-Site=Lax");
        break;
    case HttpStrict:
        connection_printf(connection, "; Same-Site=Strict");
        break;
    default:
        eprintf("Invalid Same-Site value! (value: %u)\n",
                cookie_request->same_site);
        exit(-1);
    }
    connection_printf(connection, "\n");
}

struct CookieBuilder {
    CookieRequest *cookie;
};

CookieBuilder *cookie_builder_new(String *name, String *value) {
    CookieBuilder *result = (CookieBuilder *)malloc(sizeof(CookieBuilder));
    result->cookie = (CookieRequest *)malloc(sizeof(CookieRequest));
    result->cookie->name = name;
    result->cookie->value = value;
    result->cookie->expires = optional_new(time_t);
    result->cookie->max_age = optional_new(time_t);
    result->cookie->secure = false;
    result->cookie->http_only = false;
    result->cookie->domain = optional_new(String *);
    result->cookie->path = optional_new(String *);
    result->cookie->same_site = HttpLax;
    return result;
}

void cookie_builder_set_expires(CookieBuilder *cookie_builder, time_t value) {
    optional_set(cookie_builder->cookie->expires, value);
}

void cookie_builder_unset_expires(CookieBuilder *cookie_builder) {
    optional_reset(cookie_builder->cookie->expires);
}

void cookie_builder_set_max_age(CookieBuilder *cookie_builder, time_t value) {
    optional_set(cookie_builder->cookie->max_age, value);
}

void cookie_builder_unset_max_age(CookieBuilder *cookie_builder) {
    optional_reset(cookie_builder->cookie->max_age);
}

void cookie_builder_set_secure(CookieBuilder *cookie_builder, bool value) {
    cookie_builder->cookie->secure = value;
}

void cookie_builder_set_http_only(CookieBuilder *cookie_builder, bool value) {
    cookie_builder->cookie->http_only = value;
}

void cookie_builder_set_domain(CookieBuilder *cookie_builder, String *value) {
    optional_map(cookie_builder->cookie->domain, cookie_free_optional_string);
    optional_set(cookie_builder->cookie->domain, value);
}

void cookie_builder_unset_domain(CookieBuilder *cookie_builder) {
    optional_map(cookie_builder->cookie->domain, cookie_free_optional_string);
    optional_reset(cookie_builder->cookie->domain);
}

void cookie_builder_set_path(CookieBuilder *cookie_builder, String *value) {
    optional_map(cookie_builder->cookie->path, cookie_free_optional_string);
    optional_set(cookie_builder->cookie->path, value);
}

void cookie_builder_unset_path(CookieBuilder *cookie_builder) {
    optional_map(cookie_builder->cookie->path, cookie_free_optional_string);
    optional_reset(cookie_builder->cookie->path);
}

void cookie_builder_set_same_site(CookieBuilder *cookie_builder,
                                  HttpSameSite value) {
    cookie_builder->cookie->same_site = value;
}

CookieRequest *cookie_builder_build(CookieBuilder *cookie_builder) {
    CookieRequest *result = cookie_builder->cookie;
    free(cookie_builder);
    return result;
}