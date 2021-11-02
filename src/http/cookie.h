#pragma once

#include "../utils/string.h"
#include "socket.h"

#include <time.h>

typedef struct CookieRequest CookieRequest;

#define VECTORTYPE       CookieRequest
#define VECTORFUNCNAME   cookie_request
#define VECTORSTRUCTNAME CookieRequest
#define DECLARE_VECTOR
#include "../utils/Vector.h"

void cookie_request_free(CookieRequest *cookie_request);

void cookie_print_to_connection(CookieRequest *cookie_request,
                                Connection *connection);

typedef enum HttpSameSite {
    HttpNone,
    HttpLax,
    HttpStrict,
} HttpSameSite;

typedef struct CookieBuilder CookieBuilder;

CookieBuilder *cookie_builder_new(String *name, String *value);

void cookie_builder_set_expires(CookieBuilder *cookie_builder, time_t value);

void cookie_builder_unset_expires(CookieBuilder *cookie_builder);

void cookie_builder_set_max_age(CookieBuilder *cookie_builder, time_t value);

void cookie_builder_unset_max_age(CookieBuilder *cookie_builder);

void cookie_builder_set_secure(CookieBuilder *cookie_builder, bool value);

void cookie_builder_set_http_only(CookieBuilder *cookie_builder, bool value);

void cookie_builder_set_domain(CookieBuilder *cookie_builder, String *value);

void cookie_builder_unset_domain(CookieBuilder *cookie_builder);

void cookie_builder_set_path(CookieBuilder *cookie_builder, String *value);

void cookie_builder_unset_path(CookieBuilder *cookie_builder);

void cookie_builder_set_same_site(CookieBuilder *cookie_builder,
                                  HttpSameSite value);

CookieRequest *cookie_builder_build(CookieBuilder *cookie_builder);

uint64_t cookie_hash(void *data);

int cookie_cmp(void *key1, void *key2);
