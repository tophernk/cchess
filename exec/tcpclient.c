#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

#include <stream_client.h>

int main(int argc, char **argv) {
    // create client socket
    int client_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sd == -1) {
        fprintf(stderr, "could not create client socket: %s\n", strerror(errno));
        exit(1);
    }

    // get host address
    char host[] = "localhost";
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
    addr.sin_port = htons(1024);

    // connect
    int result = connect(client_sd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        close(client_sd);
        fprintf(stderr, "could not connect to server %s\n", strerror(errno));
        exit(1);
    }

    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
    stream_client_request(client_sd, fen);

    return 0;
}