#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "request_eval.h"

#define REQUEST_SEPARATOR "#"

void _build_request(char request[111], char *fen, char *from, char *to, int depth);

void client_request_eval(int client_sd, char *fen, char *from, char *to, int depth) {
    char request[111];
    _build_request(request, fen, from, to, depth);
    // post request
    size_t req_len = strlen(request);
    int result = write(client_sd, request, req_len);
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

void _build_request(char *request, char *fen, char *from, char *to, int depth) {
    strcpy(request, fen);
    strcat(request, REQUEST_SEPARATOR);
    strcat(request, from);
    strcat(request, to);
    strcat(request, REQUEST_SEPARATOR);
    char depth_str[2];
    snprintf(depth_str, 2, "%d", depth);
    strcat(request, depth_str);
    strcat(request, REQUEST_SEPARATOR);
}
