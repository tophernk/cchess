#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../stream_server.h"

int main() {
    // create server socket
    int server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd == -1) {
        fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
        exit(1);
    }

    // bind socket address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(1025);

    int result = bind(server_sd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        close(server_sd);
        fprintf(stderr, "Could not bind socket address: %s\n", strerror(errno));
        exit(1);
    }

    // listen with backlog
    result = listen(server_sd, 100);
    if (result == -1) {
        close(server_sd);
        fprintf(stderr, "listen for incoming connections failed: %s\n", strerror(errno));
        exit(1);
    }

    // accept connections
    accept_forever(server_sd);

    return 0;
}