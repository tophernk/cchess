#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <stream_client.h>
#include "config.h"
#include "logger.h"

void _calculate_move(config_t *config, move_t *calculated_move, int depth);

void _execute_all_moves(config_t *config, move_t **best_path, move_t **current_path, const int total_depth, const int current_depth);

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

void _calculate_move(config_t *config, move_t *calculated_move, int depth) {
    config_t *tmp_conf = config_new();
    config_ctor(tmp_conf);
    config_copy(config, tmp_conf);

    move_t *best_path[depth];
    move_t *current_path[depth];

    for (int i = 0; i < depth; i++) {
        best_path[i] = move_new();
        move_ctor(best_path[i]);

        current_path[i] = move_new();
        move_ctor(current_path[i]);
    }

    for (int i = 0; i < depth; i++) {
        _execute_all_moves(tmp_conf, best_path, current_path, depth, i);
    }

    move_t *node_to_play = best_path[0];
    piece_t *piece_to_move = config_get_piece(config, config_is_white_to_move(config) ? WHITE : BLACK, move_get_from_position(node_to_play));
    move_set_piece_type(calculated_move, piece_get_type(piece_to_move));
    move_set_from_position(calculated_move, move_get_from_position(node_to_play));
    move_set_to_position(calculated_move, move_get_to_position(node_to_play));
    move_set_score(calculated_move, move_get_score(node_to_play));

    cchess_log("-----------------\nbest path: ");
    move_print(best_path, depth);

    for (int i = 0; i < depth; i++) {
        free(best_path[i]);
        free(current_path[i]);
    }

    config_dtor(tmp_conf);
    free(tmp_conf);
}

void _execute_all_moves(config_t *config, move_t **best_path, move_t **current_path, const int total_depth, const int current_depth) {
    // depth barrier
    if (current_depth == total_depth) {
        move_print(current_path, total_depth);
        if (move_cmpr(current_path, best_path, total_depth)) {
            move_cpy(current_path, best_path, total_depth);
        }
        return;
    }
    // game ending barrier

    //
    config_t *tmp_conf = config_new();
    config_ctor(tmp_conf);
    config_copy(config, tmp_conf);
    move_t *move = move_new();
    move_ctor(move);

    piece_t **pieces = config_get_pieces_of_active_color(config);
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (piece_get_type(pieces[i]) > NONE) {
            for (int x = 0; x < MAX_POSITIONS; x++) {
                piece_t *piece_to_move = pieces[i];
                char *available_position = piece_get_available_position(piece_to_move, x);
                if (*available_position != '-') {
                    move_set_piece_type(move, piece_get_type(piece_to_move));
                    char *currentPosition = piece_get_current_position(piece_to_move);

                    move_set_from_position(move, currentPosition);
                    move_set_to_position(move, available_position);
                    int score = config_execute_move(tmp_conf, move);

                    move_t *current_path_node = current_path[current_depth];
                    move_set_piece_type(current_path_node, piece_get_type(piece_to_move));
                    move_set_from_position(current_path_node, move_get_from_position(move));
                    move_set_to_position(current_path_node, move_get_to_position(move));
                    move_set_score(current_path_node, score);

                    _execute_all_moves(tmp_conf, best_path, current_path, total_depth, current_depth + 1);

                    config_copy(config, tmp_conf);
                } else {
                    break;
                }
            }
        }
    }

    free(move);
    config_dtor(tmp_conf);
    free(tmp_conf);
}
