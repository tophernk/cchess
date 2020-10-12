#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "eval_service.h"
#include "../../stream_server.h"

#define REQUEST_SEPARATOR "#"

int eval_board(config_t *config, int depth) {
    int eval_result = config_eval_to_depth(config, depth);
    return eval_result;
}

void eval_req_to_args(char *request, config_t *config, int *depth) {
    // fen part
    char *req_fen = strsep(&request, REQUEST_SEPARATOR);
    config_fen_in(config, req_fen);
    // depth part
    char *req_depth = strsep(&request, REQUEST_SEPARATOR);
    *depth = atoi(req_depth);
}

void eval_result_to_rsp(char *response, int eval_result) {
    snprintf(response, 4, "%d", eval_result);
}

void *request_handler(void *arg) {
    int sd = *((int *) arg);
    free((int *) arg);

    config_t *config = config_new();
    move_t *move = move_new();
    move_ctor(move);
    int *depth = (int *) malloc(sizeof(int));

    char buffer[111];
    while (1) {
        int result = read(sd, buffer, 111);
        if (result == 0 || result == -1) {
            break;
        }
        eval_req_to_args(buffer, config, depth);
        int eval_result = eval_board(config, *depth);
        printf("eval=%d\n", eval_result);
        char response[4];
        eval_result_to_rsp(response, eval_result);
        write(sd, response, 4);
    }
    config_dtor(config);
    free(depth);
    free(move);
    free(config);
    return NULL;
}

void accept_forever(int server_sd) {
    while (1) {
        int client_sd = accept(server_sd, NULL, NULL);
        if (client_sd == -1) {
            close(server_sd);
            fprintf(stderr, "could not accept client: %s\n", strerror(errno));
            exit(1);
        }
        int *arg = (int *) malloc(sizeof(int));
        *arg = client_sd;
        pthread_t request_handler_thread;
        int result = pthread_create(&request_handler_thread, NULL, request_handler, arg);
        if (result) {
            close(client_sd);
            close(server_sd);
            free(arg);
            fprintf(stderr, "could not start client thread\n");
            exit(1);
        }
    }
}
