#pragma once

#include "../utils/string.h"

#include <stdbool.h>

typedef struct Socket Socket;

typedef struct Connection Connection;

Socket *socket_new(char *addr, short port);

void socket_free(Socket *socket);

bool socket_has_pending_connection(Socket *socket);

Connection *socket_get_connection(Socket *socket);

void connection_free(Connection *connection);

char *connection_get_ip(Connection *connection);

int connection_scanf(Connection *connection, char *format, ...);

int connection_printf(Connection *connection, char *format, ...);

String *connection_read_upto(Connection *connection, char *delims);

String *connection_read_line(Connection *connection);

String *connection_read_word(Connection *connection);

String *connection_read_len(Connection *connection, size_t len);
