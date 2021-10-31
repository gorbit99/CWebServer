#pragma once

#include "../utils/string.h"
#include "cookie.h"
#include "response_codes.h"
#include "socket.h"
#include "version.h"

typedef struct HttpResponse HttpResponse;

void response_send_to_connection(HttpResponse *response,
                                 Connection *connection);

void response_free(HttpResponse *response);

typedef struct HttpResponseBuilder HttpResponseBuilder;

HttpResponse *response_builder_build(HttpResponseBuilder *response_builder);

HttpResponseBuilder *response_builder_new(void)
#if GCC
        __attribute__((malloc(response_builder_build), returns_nonnull))
#endif
        ;

void response_builder_set_version(HttpResponseBuilder *response_builder,
                                  HttpVersion version);

void response_builder_set_code(HttpResponseBuilder *response_builder,
                               HttpResponseCode code,
                               bool set_status);

void response_builder_set_header(HttpResponseBuilder *response_builder,
                                 String *name,
                                 String *value);

void response_builder_set_body(HttpResponseBuilder *response_builder,
                               String *body);

void response_builder_remove_body(HttpResponseBuilder *response_builder);

void response_builder_set_status(HttpResponseBuilder *response_builder,
                                 String *status);

void response_builder_add_set_cookie(HttpResponseBuilder *response_builder,
                                     CookieRequest *cookie_request);
