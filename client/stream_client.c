#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <stream_client.h>

void stream_client_request(int client_sd, char *fen) {
    // post request
    size_t req_len = strlen(fen);
    int result = write(client_sd, fen, req_len);
    if (result == -1) {
        fprintf(stderr, "error while posting request. %s\n", strerror(errno));
        exit(1);
    }

    printf("request sent (%s).\n", fen);

    // wait for response
    char response[100];
    result = read(client_sd, response, 100);
    if (result < 0) {
        fprintf(stderr, "error reading response. %s\n", strerror(errno));
    }
    if (result == 0) {
    }
    printf("response: %s\n", response);
}

