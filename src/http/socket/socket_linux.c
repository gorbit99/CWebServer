#include "socket_linux.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <sys/socket.h>

struct Socket {
    int socket_fd;
};

Socket *socket_new() {
    Socket *result = (Socket *)malloc(sizeof(Socket));

    result->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
}
