#pragma once

#include <stdbool.h>

typedef struct Socket Socket;

typedef struct Connection Connection;

Socket *socket_new(char *addr, short port);

void socket_free(Socket *socket);

bool socket_has_pending_connection(Socket *socket);

Connection *socket_get_connection(Socket *socket);

void connection_free(Connection *connection);

char *connection_get_ip(Connection *connection);
