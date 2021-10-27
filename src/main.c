#include "http/request.h"
#include "http/response.h"
#include "http/socket.h"
#include "tests/framework.h"
#include "utils/Hashmap.h"
#include "utils/Optional.h"
#include "utils/Vector.h"
#include "utils/string.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

TEST(test_suite) {
    STARTTEST();

    RUNTEST(optional);
    RUNTEST(vector);
    RUNTEST(hashmap);
    RUNTEST(string);

    ENDTEST();
}

bool running = true;

void sigint_handler(int signal) {
    assert(signal == SIGINT);
    running = false;
}

int main(void) {
#ifdef TESTING
    test_test_suite();
#else
    Socket *socket = socket_new("localhost", 8000);

    struct sigaction int_handler;
    int_handler.sa_handler = sigint_handler;
    int_handler.sa_flags = 0;
    sigemptyset(&int_handler.sa_mask);
    sigaction(SIGINT, &int_handler, NULL);

    while (running) {
        while (!socket_has_pending_connection(socket) && running) {
        }

        if (!running) {
            break;
        }

        Connection *connection = socket_get_connection(socket);

        char *hostname = connection_get_ip(connection);

        printf("Hostname: %s\n", hostname);

        HttpRequest *request = request_read(connection);

        request_print(request);

        request_free(request);

        free(hostname);

        HttpResponseBuilder *response_builder = response_builder_new();
        String *body = string_from_cstr("Hello World");
        response_builder_set_body(response_builder, body);
        HttpResponse *response = response_builder_build(response_builder);
        response_send_to_connection(response, connection);
        response_free(response);

        connection_free(connection);
    }

    socket_free(socket);
    return 0;
#endif
}
