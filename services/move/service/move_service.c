#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>

#include "move_service.h"
#include "../../eval/request/request_eval.h"
#include "../../stream_server.h"

int _connect(const char *host, const int port);

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
    int sd = _connect("evalservice", 1025);
    piece_t **pieces = config_get_pieces_of_active_color(config);
    piece_t *current_piece;
    char fen[100];
    bool white_to_move = config_is_white_to_move(config);
    int best_eval = white_to_move ? -9999 : 9999;
    move_t *best_move = move_new();
    move_ctor(best_move);
    config_fen_out(config, fen);
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        current_piece = pieces[i];
        char *from_pos = piece_get_current_position(current_piece);
        if(*from_pos == '-') {
            break;
        }
        for (int ii = 0; ii < MAX_POSITIONS; ii++) {
            char *to_pos = piece_get_available_position(current_piece, ii);
            if(*to_pos == '-') {
                break;
            }
            int eval = client_request_eval(sd, fen, from_pos, to_pos, depth);
            bool better = white_to_move ? eval > best_eval : eval < best_eval;
            if (better) {
                move_set_from_position(best_move, from_pos);
                move_set_to_position(best_move, to_pos);
            }
        }
    }

    int *e = (int *) malloc(sizeof(int));
    config_execute_move(config, best_move, e);

    free(e);
    free(best_move);
}

int _connect(const char *host, const int port) {
    // create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
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
    addr.sin_port = htons(port);

    // connect
    int result = connect(sd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        close(sd);
        fprintf(stderr, "could not connect to server %s\n", strerror(errno));
        exit(1);
    }
    return sd;
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