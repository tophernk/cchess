#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "cchess.h"
#include "config_p.h"
#include "move.h"
#include "logger.h"

#define FEN_SEPARATOR ' '
#define FEN_SEPARATOR_RANK '/'

void __determine_available_positions(piece_t *, config_t *);

int __abs(int);

void __execute_all_moves(config_t *config, piece_color_t color_to_move, move_t **best_path, move_t **current_path, int current_depth);

bool __is_pawn(piece_t *piece);

int __can_long_castle(piece_color_t, config_t *pConfig);

int __can_short_castle(piece_color_t color, config_t *pConfig);

void __move_en_passant(config_t *conf, piece_t *piece, int xto);

bool __castle_king_on_first_rank(int yfrom, piece_color_t color);

bool __castle_long(int xfrom, int xto);

bool __field_is_blocked(int x, int y, const config_t *pConfig);

bool __field_is_attacked(int x, int y, piece_color_t attacking_color, config_t *pConfig);

int __castle_clear_king_path_without_checks(int xfrom, int xto, int xstep, piece_color_t color, config_t *pConfig, int y, int success_result);

void __castle_rook_move(config_t *pConfig, piece_color_t color, int xfrom, int xto);

void __config_update_castle_flags(config_t *conf, int xfrom, piece_type_t *type);

void __fen_parse_board(config_t *config, const char *fen, int rank, int *x, int *white_i, int *black_i, int i);

void __determine_pawn_moves(piece_t *piece, config_t *config);

void __determine_knight_moves(piece_t *piece, config_t *config);

void __determine_bishop_moves(piece_t *piece, config_t *config);

void __determine_rook_moves(piece_t *piece, config_t *config);

void __determine_queen_moves(piece_t *piece, config_t *config);

void __determine_king_moves(piece_t *piece, config_t *config);

bool _index_in_bounds(int index);

config_t *config_new() {
    return (config_t *) malloc(sizeof(config_t));
}

void config_ctor(config_t *config) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            config->board[x][y] = NONE;
        }
    }
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        config->white[i] = piece_new();
        piece_ctor(config->white[i]);

        config->black[i] = piece_new();
        piece_ctor(config->black[i]);
    }

    config->check_black = false;
    config->check_white = false;
    config->short_castles_black = false;
    config->short_castles_white = false;
    config->long_castles_black = false;
    config->long_castles_white = false;

    config->enpassant[0] = '-';
    config->enpassant[1] = '-';
}

void config_dtor(config_t *config) {
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        free(config->white[i]);
        free(config->black[i]);
    }
}

void config_fen_in(config_t *config, char *fen) {
    config_ctor(config);
    int len = strlen(fen);
    int rank = 0;
    int x = 0;
    int white_i = 0;
    int black_i = 0;
    int sep_i = 0;
    int en_passant_i = 0;

    for (int i = 0; i < len; i++) {
        char value = fen[i];
        if (value == FEN_SEPARATOR_RANK) {
            x = 0;
            rank++;
            continue;
        }
        if (value == FEN_SEPARATOR) {
            sep_i++;
            continue;
        }
        if (sep_i == 0 && rank < 8 && x < 8) {
            __fen_parse_board(config, fen, rank, &x, &white_i, &black_i, i);
            continue;
        }
        if (sep_i == 1) {
            // w/b -> white or black turn
            continue;
        }
        if (sep_i == 2) {
            // castles
            if (value == 'K') {
                config->short_castles_white = true;
            } else if (value == 'Q') {
                config->long_castles_white = true;
            } else if (value == 'k') {
                config->short_castles_black = true;
            } else if (value == 'q') {
                config->long_castles_black = true;
            }
            continue;
        }
        if (sep_i == 3) {
            // en passant field
            config->enpassant[en_passant_i] = value;
            en_passant_i++;
            continue;
        }
        if (sep_i == 4) {
            // meta: half move #
            continue;
        }
        if (sep_i == 5) {
            // meta: full move #
            continue;
        }
        printf(" undefined");
    }
    config_update_available_positions(config);
}

void __fen_parse_board(config_t *config, const char *fen, int rank, int *x, int *white_i, int *black_i, int i) {
    piece_type_t piece = piece_char_to_type(fen[i]);
    if (piece != NONE) {
        piece_color_t color = piece_get_color(piece);
        config_add_piece(config, piece, *x, rank, color, color == WHITE ? (*white_i)++ : (*black_i)++);
        (*x)++;
    } else {
        (*x) += fen[i] - '0';
    }
}

void config_add_piece(config_t *config, piece_type_t type, int x, int y, piece_color_t color, int index) {
    config->board[x][y] = type;

    piece_t *piece = color == BLACK ? config->black[index] : config->white[index];
    piece_set_type(piece, type);
    piece_set_current_position(piece, x, y);
    piece_invalidate_available_positions(piece);
}

void config_update_available_positions(config_t *conf) {
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (piece_get_type(conf->white[i]) > NONE)
            __determine_available_positions(conf->white[i], conf);
        if (piece_get_type(conf->black[i]) > NONE)
            __determine_available_positions(conf->black[i], conf);
    }

    // update check flags
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            piece_type_t piece = conf->board[x][y];
            if (piece == KING_B && __field_is_attacked(x, y, WHITE, conf)) {
                conf->check_black = true;
            } else if (piece == KING_W && __field_is_attacked(x, y, BLACK, conf)) {
                conf->check_white = true;
            }
        }
    }

    if (conf->check_white || conf->check_black) {
        cchess_log("check!\n");
    }
}


int config_eval(config_t *config, piece_color_t color) {
    int white = 0;
    int black = 0;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        white += piece_type_get_weight(piece_get_type(config->white[i]));
        black += piece_type_get_weight(piece_get_type(config->black[i]));
    }
    return color == BLACK ? black - white : white - black;
}

void __determine_available_positions(piece_t *piece, config_t *config) {

    switch (piece_get_type(piece)) {
        case NONE:
            break;
        case PAWN_B:
        case PAWN_W: {
            __determine_pawn_moves(piece, config);
            break;
        }
        case KNIGHT_B:
        case KNIGHT_W:
            __determine_knight_moves(piece, config);
            break;
        case BISHOP_B:
        case BISHOP_W:
            __determine_bishop_moves(piece, config);
            break;
        case ROOK_B:
        case ROOK_W:
            __determine_rook_moves(piece, config);
            break;
        case KING_B:
        case KING_W:
            __determine_king_moves(piece, config);
            break;
        case QUEEN_B:
        case QUEEN_W:
            __determine_queen_moves(piece, config);
            break;
    }
}

void __determine_king_moves(piece_t *piece, config_t *config) {
    int valid_pos_i = 0;
    char *position = piece_get_current_position(piece);
    char available_position[2];
    int x = position_get_x(position);
    int y = position_get_y(position);
    piece_color_t color = piece_get_color(piece_get_type(piece));
    piece_color_t opposite_color = color == WHITE ? BLACK : WHITE;

    // test the 8 directions clockwise
    int x_direction[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    int y_direction[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    for (int i = 0; i < 8; i++) {
        int current_x = x;
        int current_y = y;
        current_x += x_direction[i];
        current_y += y_direction[i];
        if (_index_in_bounds(current_x) && _index_in_bounds(current_y)) {
            piece_type_t piece_at_target = config->board[current_x][current_y];
            if (piece_at_target == NONE) {
                position_set_file_rank(available_position, current_x, current_y);
                piece_set_available_position_new(piece, available_position, valid_pos_i++);
            } else if (piece_get_color(piece_at_target) == opposite_color) {
                position_set_file_rank(available_position, current_x, current_y);
                piece_set_available_position_new(piece, available_position, valid_pos_i++);
                break;
            } else {
                break;
            }
        }
    }
    // test castles if starting position
    char starting_rank = color == WHITE ? '1' : '8';
    if (position[0] == 'e' && position[1] == starting_rank) {
        if (__can_short_castle(color, config)) {
            available_position[0] = 'g';
            available_position[1] = color == WHITE ? '1' : '8';
            piece_set_available_position_new(piece, available_position, valid_pos_i++);
        }
        if (__can_long_castle(color, config)) {
            available_position[0] = 'c';
            available_position[1] = color == WHITE ? '1' : '8';
            piece_set_available_position_new(piece, available_position, valid_pos_i++);
        }
    }
    // invalidate remaining positions
    while (valid_pos_i < MAX_POSITIONS) {
        position_invalidate(available_position);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
}

void __determine_queen_moves(piece_t *piece, config_t *config) {
    int valid_pos_i = 0;
    char *position = piece_get_current_position(piece);
    char available_position[2];
    int x = position_get_x(position);
    int y = position_get_y(position);
    piece_color_t opposite_color = piece_get_color(piece_get_type(piece)) == WHITE ? BLACK : WHITE;

    // test the 8 directions clockwise
    int x_direction[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    int y_direction[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    for (int i = 0; i < 8; i++) {
        int current_x = x;
        int current_y = y;
        for (int ii = 0; ii < 8; ii++) {
            current_x += x_direction[i];
            current_y += y_direction[i];
            if (_index_in_bounds(current_x) && _index_in_bounds(current_y)) {
                piece_type_t piece_at_target = config->board[current_x][current_y];
                if (piece_at_target == NONE) {
                    position_set_file_rank(available_position, current_x, current_y);
                    piece_set_available_position_new(piece, available_position, valid_pos_i++);
                } else if (piece_get_color(piece_at_target) == opposite_color) {
                    position_set_file_rank(available_position, current_x, current_y);
                    piece_set_available_position_new(piece, available_position, valid_pos_i++);
                    break;
                } else {
                    break;
                }
            }
        }
    }
    // invalidate remaining positions
    while (valid_pos_i < MAX_POSITIONS) {
        position_invalidate(available_position);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
}

void __determine_rook_moves(piece_t *piece, config_t *config) {
    int valid_pos_i = 0;
    char *position = piece_get_current_position(piece);
    char available_position[2];
    int x = position_get_x(position);
    int y = position_get_y(position);
    piece_color_t opposite_color = piece_get_color(piece_get_type(piece)) == WHITE ? BLACK : WHITE;

    // test the four directions clockwise
    int x_direction[4] = {1, 0, -1, 0};
    int y_direction[4] = {0, 1, 0, -1};
    for (int i = 0; i < 4; i++) {
        int current_x = x;
        int current_y = y;
        for (int ii = 0; ii < 8; ii++) {
            current_x += x_direction[i];
            current_y += y_direction[i];
            if (_index_in_bounds(current_x) && _index_in_bounds(current_y)) {
                piece_type_t piece_at_target = config->board[current_x][current_y];
                if (piece_at_target == NONE) {
                    position_set_file_rank(available_position, current_x, current_y);
                    piece_set_available_position_new(piece, available_position, valid_pos_i++);
                } else if (piece_get_color(piece_at_target) == opposite_color) {
                    position_set_file_rank(available_position, current_x, current_y);
                    piece_set_available_position_new(piece, available_position, valid_pos_i++);
                    break;
                } else {
                    break;
                }
            }
        }
    }
    // invalidate remaining positions
    while (valid_pos_i < MAX_POSITIONS) {
        position_invalidate(available_position);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
}

void __determine_bishop_moves(piece_t *piece, config_t *config) {
    int valid_pos_i = 0;
    char *position = piece_get_current_position(piece);
    char available_position[2];
    int x = position_get_x(position);
    int y = position_get_y(position);
    piece_color_t opposite_color = piece_get_color(piece_get_type(piece)) == WHITE ? BLACK : WHITE;

    // test the four diagonals clockwise
    int x_direction[4] = {1, 1, -1, -1};
    int y_direction[4] = {-1, 1, 1, -1};
    for (int i = 0; i < 4; i++) {
        int current_x = x;
        int current_y = y;
        for (int ii = 0; ii < 8; ii++) {
            current_x += x_direction[i];
            current_y += y_direction[i];
            if (_index_in_bounds(current_x) && _index_in_bounds(current_y)) {
                piece_type_t piece_at_target = config->board[current_x][current_y];
                if (piece_at_target == NONE) {
                    position_set_file_rank(available_position, current_x, current_y);
                    piece_set_available_position_new(piece, available_position, valid_pos_i++);
                } else if (piece_get_color(piece_at_target) == opposite_color) {
                    position_set_file_rank(available_position, current_x, current_y);
                    piece_set_available_position_new(piece, available_position, valid_pos_i++);
                    break;
                } else {
                    break;
                }
            }
        }
    }
    // invalidate remaining positions
    while (valid_pos_i < MAX_POSITIONS) {
        position_invalidate(available_position);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
}

void __determine_knight_moves(piece_t *piece, config_t *config) {
    int valid_pos_i = 0;
    char *position = piece_get_current_position(piece);
    char available_position[2];
    int x = position_get_x(position);
    int y = position_get_y(position);
    piece_color_t opposite_color = piece_get_color(piece_get_type(piece)) == WHITE ? BLACK : WHITE;

    // test L shape moves clockwise
    int xto[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    int yto[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    for (int i = 0; i < 8; i++) {
        int current_x = x + xto[i];
        int current_y = y + yto[i];
        if (_index_in_bounds(current_x) && _index_in_bounds(current_y)) {
            piece_type_t piece_at_target = config->board[current_x][current_y];
            if (piece_at_target == NONE || piece_get_color(piece_at_target) == opposite_color) {
                position_set_file_rank(available_position, current_x, current_y);
                piece_set_available_position_new(piece, available_position, valid_pos_i++);
            }
        }
    }
    // invalidate remaining positions
    while (valid_pos_i < MAX_POSITIONS) {
        position_invalidate(available_position);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
}

void __determine_pawn_moves(piece_t *piece, config_t *config) {
    int valid_pos_i = 0;
    char *position = piece_get_current_position(piece);
    char available_position[2];
    int x = position_get_x(position);
    int y = position_get_y(position);

    bool is_white = piece_get_color(piece_get_type(piece)) == WHITE;
    int y_direction = is_white ? -1 : 1;
    char start_rank = is_white ? '2' : '7';
    char enpassant_rank = is_white ? '5' : '4';
    piece_color_t opposite_color = is_white ? BLACK : WHITE;

    // double forward move from starting position
    if (position[1] == start_rank && config->board[x][y + 2 * y_direction] == NONE) {
        position_set_file_rank(available_position, x, y + 2 * y_direction);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
    // en passant move
    if (position[1] == enpassant_rank && position_valid(config->enpassant) && __abs(position_get_x(config->enpassant) - x) == 1 &&
        position_get_y(config->enpassant) - y == y_direction) {
        piece_set_available_position_new(piece, config->enpassant, valid_pos_i++);
    }
    // standard forward move
    int y_standard_move = y + 1 * y_direction;
    if (_index_in_bounds(y_standard_move) && config->board[x][y_standard_move] == NONE) {
        position_set_file_rank(available_position, x, y_standard_move);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
    // takes to left and right
    int x_takes = x + 1;
    for (int i = 0; i < 2; i++) {
        if (_index_in_bounds(x_takes) && _index_in_bounds(y_standard_move) && piece_get_color(config->board[x_takes][y_standard_move]) == opposite_color) {
            position_set_file_rank(available_position, x_takes, y_standard_move);
            piece_set_available_position_new(piece, available_position, valid_pos_i++);
            x_takes = x_takes * -1;
        }
    }
    // invalidate remaining positions
    while (valid_pos_i < MAX_POSITIONS) {
        position_invalidate(available_position);
        piece_set_available_position_new(piece, available_position, valid_pos_i++);
    }
}

bool _index_in_bounds(int index) { return index > -1 && index < BOARD_SIZE; }

void __execute_all_moves(config_t *config, piece_color_t color_to_move, move_t **best_path, move_t **current_path, int current_depth) {
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

    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_t **pieces = color_to_move == WHITE ? tmp_conf->white : tmp_conf->black;
        if (piece_get_type(pieces[i]) > NONE) {
            for (int x = 0; x < MAX_POSITIONS; x++) {
                piece_t *piece_to_move = color_to_move == WHITE ? tmp_conf->white[i] : tmp_conf->black[i];
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

                    __execute_all_moves(tmp_conf, color_to_move == WHITE ? BLACK : WHITE, best_path, current_path, current_depth + 1);

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

int __can_short_castle(piece_color_t color, config_t *pConfig) {
    int y = color == WHITE ? 7 : 0;
    if (color == BLACK && pConfig->short_castles_black) {
        return __castle_clear_king_path_without_checks(4, 6, 1, BLACK, pConfig, y, 3);
    } else if (color == WHITE && pConfig->short_castles_white) {
        return __castle_clear_king_path_without_checks(4, 6, 1, WHITE, pConfig, y, 4);
    }
    return 0;
}

int __castle_clear_king_path_without_checks(int xfrom, int xto, int xstep, piece_color_t color, config_t *pConfig, int y, int success_result) {
    piece_color_t attacking_color = color == WHITE ? BLACK : WHITE;
    for (int x = xfrom + xstep; x <= xto; x += xstep) {
        if (__field_is_blocked(x, y, pConfig) || __field_is_attacked(x, y, attacking_color, pConfig)) {
            return 0;
        }
    }
    return success_result;
}

bool __field_is_blocked(int x, int y, const config_t *pConfig) {
    return pConfig->board[x][y] > NONE;
}

bool __field_is_attacked(int x, int y, piece_color_t attacking_color, config_t *pConfig) {
    piece_t **attackers = attacking_color == WHITE ? pConfig->white : pConfig->black;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_t *current_attacker = attackers[i];
        for (int ii = 0; ii < MAX_POSITIONS; ii++) {
            char *position = piece_get_available_position(current_attacker, ii);
            if (position_to_x(position[0]) == x && position_to_y(position[1]) == y) {
                return true;
            }
        }
    }
    return false;
}

int __can_long_castle(piece_color_t color, config_t *pConfig) {
    int y = color == WHITE ? 7 : 0;
    if (pConfig->board[1][y] > NONE) {
        return 0;
    }
    if (color == BLACK && pConfig->long_castles_black) {
        return __castle_clear_king_path_without_checks(4, 2, -1, BLACK, pConfig, y, 5);
    } else if (color == WHITE && pConfig->long_castles_white) {
        return __castle_clear_king_path_without_checks(4, 2, -1, WHITE, pConfig, y, 6);
    }
    return 0;
}

int __abs(int x) {
    if (x < 0)
        return x * -1;
    return x;
}

int config_execute_move(config_t *conf, move_t *move) {
    char *from = move_get_from_position(move);
    char *to = move_get_to_position(move);

    config_t *backup_config = config_new();
    config_ctor(backup_config);
    config_copy(conf, backup_config);

    int move_executed = 0;
    conf->check_white = 0;
    conf->check_black = 0;

    if (*from != '-' && *to != '-') {
        piece_color_t move_color = piece_get_color(move_get_piece_type(move));
        piece_t *piece = config_get_piece(conf, move_color, from);
        int valid_move = piece_valid_move(piece, to);
        if (valid_move) {
            conf->board[position_to_x(from[0])][position_to_y(from[1])] = NONE;

            int xto = position_get_x(to);
            int yto = position_get_y(to);
            if (conf->board[xto][yto] != NONE) {
                config_remove_piece(conf, to);
            } else if (config_en_passant(conf, to) && __is_pawn(piece)) {
                __move_en_passant(conf, piece, xto);
            }
            piece_type_t type = piece_get_type(piece);
            if (valid_move == 2) {
                position_set_file_rank(conf->enpassant, xto, piece_get_color(type) == WHITE ? 5 : 2);
            } else {
                position_invalidate(conf->enpassant);
            }
            if (valid_move == 3 || valid_move == 4) {
                __castle_rook_move(conf, move_color, 7, 5);
            }
            if (valid_move == 5 || valid_move == 6) {
                __castle_rook_move(conf, move_color, 0, 3);
            }
            __config_update_castle_flags(conf, position_to_x(from[0]), &type);

            piece_set_current_position(piece, xto, yto);
            conf->board[xto][yto] = type;

            config_update_available_positions(conf);

            int revert_move = 0;
            if (move_color == BLACK && conf->check_black) {
                revert_move = 1;
            }
            if (move_color == WHITE && conf->check_white) {
                revert_move = 1;
            }
            if (revert_move) {
                //revert
                move_executed = 0;
                config_copy(backup_config, conf);
            } else {
                move_executed = 1;
            }
        }
    }

    config_dtor(backup_config);
    free(backup_config);

    return move_executed ? config_eval(conf, BLACK) : -9999;
}

void __config_update_castle_flags(config_t *conf, int xfrom, piece_type_t *type) {
    if ((*type) == KING_W) {
        config_disable_short_castles(conf, WHITE);
        config_disable_long_castles(conf, WHITE);
    } else if ((*type) == KING_B) {
        config_disable_short_castles(conf, BLACK);
        config_disable_long_castles(conf, BLACK);
    } else if ((*type) == ROOK_W && xfrom == 0) {
        config_disable_long_castles(conf, WHITE);
    } else if ((*type) == ROOK_W && xfrom == 7) {
        config_disable_short_castles(conf, WHITE);
    } else if ((*type) == ROOK_B && xfrom == 0) {
        config_disable_long_castles(conf, BLACK);
    } else if ((*type) == ROOK_B && xfrom == 7) {
        config_disable_short_castles(conf, BLACK);
    }
}

void __castle_rook_move(config_t *pConfig, piece_color_t color, int xfrom, int xto) {
    int y = color == BLACK ? 0 : 7;
    char position[2];
    position_set_file_rank(position, xfrom, y);
    piece_t *pPiece = config_get_piece(pConfig, color, position);

    char *rook_position = piece_get_current_position(pPiece);
    rook_position[0] = position_to_file(xto);
    pConfig->board[xfrom][y] = NONE;
    pConfig->board[xto][y] = color == BLACK ? ROOK_B : ROOK_W;
}

void __move_en_passant(config_t *conf, piece_t *piece, int xto) {
    int en_passant_piece_rank = piece_get_color(piece_get_type(piece)) == WHITE ? 3 : 4;

    char en_passant_piece_position[2];
    position_set_file_rank(en_passant_piece_position, xto, en_passant_piece_rank);

    config_remove_piece(conf, en_passant_piece_position);
    conf->board[xto][en_passant_piece_rank] = NONE;
}

bool __is_pawn(piece_t *piece) {
    piece_type_t type = piece_get_type(piece);
    return type == PAWN_B || type == PAWN_W;
}

void config_remove_piece(config_t *cfg, char *position) {
    int x = position_to_x(position[0]);
    int y = position_to_y(position[1]);
    piece_type_t piece_type = cfg->board[x][y];
    piece_t **piece = NULL;
    if (piece_type < 7) {
        piece = cfg->white;
    } else {
        piece = cfg->black;
    }
    char *currentPosition = piece_get_current_position(*piece);
    while (currentPosition[0] != position[0] || currentPosition[1] != position[1]) {
        piece++;
        currentPosition = piece_get_current_position(*piece);
    }
    position_invalidate(currentPosition);
    piece_set_type(*piece, NONE);
    piece_invalidate_available_positions(*piece);
}

int config_move_cpu(config_t *conf) {
    printf("cpu move...\n");
    int result = 0;
    move_t *next_move = move_new();
    move_ctor(next_move);
    config_calculate_move(conf, next_move);

    if (move_get_piece_type(next_move) > NONE && position_valid(move_get_to_position(next_move))) {
        result = config_execute_move(conf, next_move);
    }
    config_print(conf);

    free(next_move);

    return MOVE_EXECUTED(result);
}

void config_calculate_move(config_t *conf, move_t *calculated_move) {
    config_t *tmp_conf = config_new();
    config_ctor(tmp_conf);
    config_copy(conf, tmp_conf);

    move_t *best_path[DEPTH];
    move_t *current_path[DEPTH];

    for (int i = 0; i < DEPTH; i++) {
        best_path[i] = move_new();
        move_ctor(best_path[i]);

        current_path[i] = move_new();
        move_ctor(current_path[i]);
    }

    for (int i = 0; i < DEPTH; i++) {
        piece_color_t turn_color = i % 2 == 0 ? BLACK : WHITE;
        __execute_all_moves(tmp_conf, turn_color, best_path, current_path, i);
    }

    move_t *node_to_play = best_path[0];
    piece_t *piece_to_move = config_get_piece(conf, BLACK, move_get_from_position(node_to_play));
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

int config_move_available(config_t *config, piece_color_t color) {
    piece_t **piece = color == WHITE ? config->white : config->black;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (piece_get_type(piece[i]) > NONE) {
            for (int x = 0; x < MAX_POSITIONS; x++) {
                if (position_valid(piece_get_available_position(piece[i], x))) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

piece_t *config_get_piece(config_t *config, piece_color_t color, char position[2]) {
    piece_t **pieces = color == WHITE ? config->white : config->black;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        char *currentPosition = piece_get_current_position(pieces[i]);
        if (currentPosition[0] == position[0] && currentPosition[1] == position[1]) {
            return pieces[i];
        }
    }
    return NULL;;
}

void config_copy(config_t *src, config_t *dst) {
    dst->check_white = src->check_white;
    dst->short_castles_white = src->short_castles_white;
    dst->long_castles_white = src->long_castles_white;
    dst->short_castles_black = src->short_castles_black;
    dst->long_castles_black = src->long_castles_black;
    position_copy(src->enpassant, dst->enpassant);

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            dst->board[x][y] = src->board[x][y];
        }

        for (int i = 0; i < NUMBER_OF_PIECES; i++) {
            piece_copy(src->white[i], dst->white[i]);
            piece_copy(src->black[i], dst->black[i]);
        }
    }
}

void config_enable_short_castles(config_t *config, piece_color_t color) {
    if (color == BLACK) {
        config->short_castles_black = 1;
    } else {
        config->short_castles_white = 1;
    }
}

void config_disable_short_castles(config_t *config, piece_color_t color) {
    if (color == BLACK) {
        config->short_castles_black = 0;
    } else {
        config->short_castles_white = 0;
    }
}

void config_enable_long_castles(config_t *config, piece_color_t color) {
    if (color == BLACK) {
        config->long_castles_black = 1;
    } else {
        config->long_castles_white = 1;
    }
}

void config_disable_long_castles(config_t *config, piece_color_t color) {
    if (color == BLACK) {
        config->long_castles_black = 0;
    } else {
        config->long_castles_white = 0;
    }
}

int config_en_passant(config_t *config, char *position) {
    return position_valid(config->enpassant) && position_equal(config->enpassant, position);
}
