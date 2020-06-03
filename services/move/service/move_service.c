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
        execute_best_move(config, 3);
        char fen_out[100];
        config_fen_out(config, fen_out);
        write(sd, fen_out, 100);
    }
    config_dtor(config);
    free(config);
    return NULL;
}

void execute_best_move(config_t *config, int depth) {
    config_t *tmp_config = config_new();
    config_copy(config, tmp_config);

    bool white_to_move = config_is_white_to_move(tmp_config);
    int best_eval = white_to_move ? -9999 : 9999;
    int *eval = (int *) malloc(sizeof(int));

    move_t *move = move_new();
    move_ctor(move);
    move_t *best_move = move_new();
    move_ctor(move);

    piece_t **pieces = config_get_pieces_of_active_color(tmp_config);
    piece_t *current_piece;

    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        current_piece = pieces[i];
        char *currentPosition = piece_get_current_position(current_piece);
        if (*currentPosition == '-') {
            break;
        }
        move_set_from_position(move, currentPosition);
        for (int ii = 0; ii < MAX_POSITIONS; ii++) {
            char *position = piece_get_available_position(current_piece, ii);
            if (*position == '-') {
                break;
            }
            move_set_to_position(move, position);
            // eval service
            config_execute_move(tmp_config, move, eval);
            int eval_result = config_eval_to_depth(tmp_config, depth);
            // #####
            bool better = white_to_move ? eval_result > best_eval : eval_result < best_eval;
            if (better) {
                move_cpy(move, best_move);
            }
            config_copy(config, tmp_config);
        }
    }
    free(move);
    free(eval);
    free(tmp_config);
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