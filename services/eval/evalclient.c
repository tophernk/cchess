#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

#include "request/request_eval.h"

int main(int argc, char **argv) {
    char *host;
    int server_port = 1025;
    if (argc > 1) {
        host = argv[1];
    } else {
        host = "localhost";
    }
    if (argc > 2) {
        server_port = 0;
        char *port = argv[2];
        int port_len = strlen(port);
        int port_digit;
        for (int i = 0; i < port_len; i++) {
            port_digit = port[i] - '0';
            server_port = server_port * 10 + port_digit;
        }
    }
    // create client socket
    int client_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sd == -1) {
        fprintf(stderr, "could not create client socket: %s\n", strerror(errno));
        exit(1);
    }

    // get host address
    struct hostent *host_entry;
    host_entry = gethostbyname(host);
    if (!host_entry) {
        fprintf(stderr, "could not resolve host name %s\n", host);
        exit(1);
    }
    // prep socket

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = *((struct in_addr *) host_entry->h_addr);
    addr.sin_port = htons(server_port);

    // connect
    int result = connect(client_sd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        close(client_sd);
        fprintf(stderr, "could not connect to server %s\n", strerror(errno));
        exit(1);
    }

    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
    client_request_eval(client_sd, fen, 2);

    return 0;
}