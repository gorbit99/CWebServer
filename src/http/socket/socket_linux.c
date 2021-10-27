#include "../../utils/Errors.h"
#include "../socket.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int SOCKET_BACKLOG = 16;

struct Socket {
    int fd;
    struct pollfd poll_fd;
};

struct Connection {
    int fd;
    struct sockaddr_in address;
    FILE *file_equivalent;
};

static int socket_create(void) {
    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (socket_fd == -1) {
        eprintf("Couldn't create socket: %s\n", strerror(errno));
        exit(-1);
    }

    return socket_fd;
}

static struct in_addr socket_resolve_hostname(char *hostname) {
    struct hostent *resolved = gethostbyname(hostname);
    if (resolved == NULL) {
        eprintf("Couldn't resolve address: %s\n", hstrerror(h_errno));
        exit(-1);
    }

    struct in_addr *ip = (struct in_addr *)resolved->h_addr_list[0];

    return *ip;
}

static struct sockaddr_in socket_create_sockaddr(char *hostname, short port) {
    struct in_addr ip = socket_resolve_hostname(hostname);

    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons((uint16_t)port);
    sockaddr.sin_addr = ip;

    return sockaddr;
}

static void socket_bind(int socket_fd, char *hostname, short port) {
    struct sockaddr_in sockaddr = socket_create_sockaddr(hostname, port);

    if (bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
        eprintf("Couldn't bind socket: %s\n", strerror(errno));
        exit(-1);
    }
}

static void socket_start_listening(int socket_fd) {
    if (listen(socket_fd, SOCKET_BACKLOG) == -1) {
        eprintf("Couldn't put socket into listening mode: %s\n",
                strerror(errno));
        exit(-1);
    }
}

static void socket_set_options(int socket_fd) {
    int value = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value))
        == -1) {
        eprintf("Couldn't set socket option: %s\n", strerror(errno));
        exit(-1);
    }
}

Socket *socket_new(char *addr, short port) {
    int socket_fd = socket_create();
    socket_set_options(socket_fd);
    socket_bind(socket_fd, addr, port);
    socket_start_listening(socket_fd);

    Socket *result = (Socket *)malloc(sizeof(Socket));
    result->fd = socket_fd;
    result->poll_fd.fd = socket_fd;
    result->poll_fd.events = POLLIN;

    return result;
}

void socket_free(Socket *socket) {
    shutdown(socket->fd, SHUT_RDWR);
    close(socket->fd);
    free(socket);
}

bool socket_has_pending_connection(Socket *socket) {
    return poll(&socket->poll_fd, 1, 0) > 0;
}

Connection *socket_get_connection(Socket *socket) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int connection_fd = accept(socket->fd, (struct sockaddr *)&addr, &addr_len);

    if (connection_fd == -1) {
        eprintf("Couldn't accept connection: %s\n", strerror(errno));
        exit(-1);
    }

    Connection *result = (Connection *)malloc(sizeof(Connection));
    result->fd = connection_fd;
    result->address = addr;

    FILE *file = fdopen(dup(connection_fd), "r+");
    result->file_equivalent = file;

    return result;
}

void connection_free(Connection *connection) {
    fclose(connection->file_equivalent);
    close(connection->fd);
    free(connection);
}

char *connection_get_ip(Connection *connection) {
    char *ip = inet_ntoa(connection->address.sin_addr);

    char *result = (char *)malloc(sizeof(char) * (strlen(ip) + 1));
    strcpy(result, ip);

    return result;
}

int connection_scanf(Connection *connection, char *format, ...) {
    va_list args;
    va_start(args, format);

    int res = vfscanf(connection->file_equivalent, format, args);

    va_end(args);

    return res;
}

int connection_printf(Connection *connection, char *format, ...) {
    va_list args;
    va_start(args, format);

    int res = vfprintf(connection->file_equivalent, format, args);

    va_end(args);

    return res;
}

String *connection_read_upto(Connection *connection, char *delims) {
    return string_read_upto(connection->file_equivalent, delims);
}

String *connection_read_line(Connection *connection) {
    return string_read_line(connection->file_equivalent);
}

String *connection_read_word(Connection *connection) {
    return string_read_word(connection->file_equivalent);
}

String *connection_read_len(Connection *connection, size_t len) {
    return string_read_len(connection->file_equivalent, len);
}
