#include <stdlib.h>

#include "logger.h"
#include "config.h"
#include "position.h"
#include "ccbot.h"

void _calculate_move(config_t *, move_t *);

void _execute_all_moves(config_t *config, move_t **best_path, move_t **current_path, int current_depth);

int ccbot_execute_move(config_t *conf) {
    cchess_log("cpu move...\n");
    int result = 0;
    move_t *next_move = move_new();
    move_ctor(next_move);
    _calculate_move(conf, next_move);

    if (move_get_piece_type(next_move) > NONE && position_valid(move_get_to_position(next_move))) {
        result = config_execute_move(conf, next_move);
    }

    free(next_move);

    return MOVE_EXECUTED(result);
}

void _calculate_move(config_t *config, move_t *calculated_move) {
    config_t *tmp_conf = config_new();
    config_ctor(tmp_conf);
    config_copy(config, tmp_conf);

    move_t *best_path[DEPTH];
    move_t *current_path[DEPTH];

    for (int i = 0; i < DEPTH; i++) {
        best_path[i] = move_new();
        move_ctor(best_path[i]);

        current_path[i] = move_new();
        move_ctor(current_path[i]);
    }

    for (int i = 0; i < DEPTH; i++) {
        _execute_all_moves(tmp_conf, best_path, current_path, i);
    }

    move_t *node_to_play = best_path[0];
    piece_t *piece_to_move = config_get_piece(config, config_is_white_to_move(config) ? WHITE : BLACK, move_get_from_position(node_to_play));
    move_set_piece_type(calculated_move, piece_get_type(piece_to_move));
    move_set_from_position(calculated_move, move_get_from_position(node_to_play));
    move_set_to_position(calculated_move, move_get_to_position(node_to_play));
    move_set_score(calculated_move, move_get_score(node_to_play));

    cchess_log("-----------------\nbest path: ");
    move_print(best_path, DEPTH);

    for (int i = 0; i < DEPTH; i++) {
        free(best_path[i]);
        free(current_path[i]);
    }

    config_dtor(tmp_conf);
    free(tmp_conf);
}

void _execute_all_moves(config_t *config, move_t **best_path, move_t **current_path, int current_depth) {
    // depth barrier
    if (current_depth == DEPTH) {
        move_print(current_path, DEPTH);
        if (move_cmpr(current_path, best_path, DEPTH)) {
            move_cpy(current_path, best_path, DEPTH);
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

                    _execute_all_moves(tmp_conf, best_path, current_path, current_depth + 1);

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
