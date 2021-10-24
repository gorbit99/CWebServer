#pragma once

#include "socket.h"
typedef struct HttpRequest HttpRequest;

HttpRequest *request_read(Connection *connection);

void request_print(HttpRequest *request);

void request_free(HttpRequest *request);
