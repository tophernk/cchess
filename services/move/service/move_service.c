#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "move_service.h"
#include "../../stream_server.h"

void *request_handler(void *arg) {
    int sd = *((int *) arg);
    free((int *) arg);
    config_t *config = config_new();
    char buffer[100];
    while (1) {
        int result = read(sd, buffer, 100);
        if (result == 0 || result == -1) {
            break;
        }
        config_fen_in(config, buffer);
        execute_best_move(config, 2);
        char fen_out[100];
        config_fen_out(config, fen_out);
        write(sd, fen_out, 100);
    }
    config_dtor(config);
    free(config);
    return NULL;
}

void execute_best_move(config_t *config, int depth) {
    config_play_best_move(config, depth);
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