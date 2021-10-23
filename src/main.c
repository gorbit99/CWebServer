#include "http/socket.h"
#include "tests/framework.h"
#include "utils/Hashmap.h"
#include "utils/Optional.h"
#include "utils/Vector.h"

#include <stdio.h>
#include <stdlib.h>

TEST(test_suite) {
    STARTTEST();

    RUNTEST(optional);
    RUNTEST(vector);
    RUNTEST(hashmap);

    ENDTEST();
}

int main(void) {
#ifdef TESTING
    test_test_suite();
#else
    Socket *socket = socket_new("localhost", 8000);

    while (true) {
        while (!socket_has_pending_connection(socket)) {
        }

        Connection *connection = socket_get_connection(socket);

        char *hostname = connection_get_ip(connection);

        printf("Hostname: %s\n", hostname);

        char line[100];
        connection_scanf(connection, "%[^\n]", line);

        printf("%s\n", line);

        free(hostname);

        connection_free(connection);
    }

    socket_free(socket);
    return 0;
#endif
}
