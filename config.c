#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "cchess.h"
#include "config_p.h"
#include "move.h"
#include "logger.h"

int __is_valid_pawn_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_knight_move(int xfrom, int yfrom, int xto, int yto);

int __is_valid_rook_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_bishop_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_queen_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_providing_check(config_t *, int xto, int yto);

void __determine_available_positions(piece_t *, config_t *);

int __abs(int);

void __execute_all_moves(config_t *config, piece_color_t color_to_move, move_t **best_path, move_t **current_path, int current_depth);

void __clear_en_passant_flag(config_t *);

bool __is_pawn(piece_t *piece);

int __is_valid_king_move(int xfrom, int yfrom, int xto, int yto, piece_type_t, config_t *config);

int __can_long_castle(int xfrom, int xto, piece_color_t, config_t *pConfig);

int __can_short_castle(int xfrom, int xto, piece_color_t color, config_t *pConfig);

void __move_en_passant(config_t *conf, piece_t *piece, int xto);

bool __castle_king_on_first_rank(int yfrom, piece_color_t color);

bool __castle_long(int xfrom, int xto);

bool __field_is_blocked(int x, int y, const config_t *pConfig);

bool __field_is_attacked(int x, int y, piece_color_t attacking_color, config_t *pConfig);

int __castle_clear_king_path_without_checks(int xfrom, int xto, int xstep, piece_color_t color, config_t *pConfig, int y, int success_result);

void __castle_rook_move(config_t *pConfig, piece_color_t color, int xfrom, int xto);

void __config_update_castle_flags(config_t *conf, int xfrom, piece_type_t *type);

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
}

void config_dtor(config_t *config) {
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_dtor(config->white[i]);
        free(config->white[i]);

        piece_dtor(config->black[i]);
        free(config->black[i]);
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
    if (conf->check_white) {
        printf("check!\n");
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

void __determine_available_positions(piece_t *piece, config_t *conf) {
    int valid_pos_counter = 0;
    for (int to_x = 0; to_x < BOARD_SIZE; to_x++) {
        for (int to_y = 0; to_y < BOARD_SIZE; to_y++) {
            if (config_valid_move(conf, piece, to_x, to_y)) {
                piece_set_available_position(piece, to_x, to_y, valid_pos_counter);
                valid_pos_counter++;
            }
        }
    }
    // invalidate remaining positions
    for (int i = valid_pos_counter; i < MAX_POSITIONS; i++) {
        piece_set_available_position(piece, -1, -1, valid_pos_counter);
    }
}

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
                position_t *available_position = piece_get_available_position(piece_to_move, x);
                if (position_get_x(available_position) != -1) {
                    move_set_piece_type(move, piece_get_type(piece_to_move));
                    move_set_from_position(move, piece_get_current_position(piece_to_move));
                    move_set_to_position(move, available_position);
                    int score = config_execute_move(tmp_conf, move);

                    move_t *current_path_node = current_path[current_depth];
                    move_set_piece_type(current_path_node, piece_get_type(piece_to_move));
                    move_set_from_position(current_path_node, move_get_from_position(move));
                    move_set_to_position(current_path_node, move_get_to_position(move));
                    move_set_score(current_path_node, score);

                    __execute_all_moves(tmp_conf, color_to_move == WHITE ? BLACK : WHITE, best_path, current_path, current_depth + 1);

                    config_copy(config, tmp_conf);
                }
            }
        }
    }

    move_dtor(move);
    free(move);

    config_dtor(tmp_conf);
    free(tmp_conf);
}

int config_valid_move(config_t *conf, piece_t *piece, int xto, int yto) {
    int result = 0;
    piece_type_t piece_at_from_position = piece_get_type(piece);
    position_t *from_position = piece_get_current_position(piece);
    int xfrom = position_get_x(from_position);
    int yfrom = position_get_y(from_position);

    piece_type_t piece_at_to_position = conf->board[xto][yto];
    if (piece_at_to_position > NONE) {
        if (piece_get_color(piece_at_from_position) == piece_get_color(piece_at_to_position)) {
            return 0;
        }
    }

    if (piece_at_from_position == PAWN_W || piece_at_from_position == PAWN_B) {
        result = __is_valid_pawn_move(xfrom, yfrom, xto, yto, conf);
    } else if (piece_at_from_position == KNIGHT_W || piece_at_from_position == KNIGHT_B) {
        result = __is_valid_knight_move(xfrom, yfrom, xto, yto);
    } else if (piece_at_from_position == BISHOP_W || piece_at_from_position == BISHOP_B) {
        result = __is_valid_bishop_move(xfrom, yfrom, xto, yto, conf);
    } else if (piece_at_from_position == ROOK_W || piece_at_from_position == ROOK_B) {
        result = __is_valid_rook_move(xfrom, yfrom, xto, yto, conf);
    } else if (piece_at_from_position == KING_W || piece_at_from_position == KING_B) {
        result = __is_valid_king_move(xfrom, yfrom, xto, yto, piece_at_from_position, conf);
    } else if (piece_at_from_position == QUEEN_W || piece_at_from_position == QUEEN_B) {
        result = __is_valid_queen_move(xfrom, yfrom, xto, yto, conf);
    }

    if (result && __is_providing_check(conf, xto, yto)) {
        if (piece_at_to_position == KING_B) {
            conf->check_black = 1;
        } else {
            conf->check_white = 1;
        }
    }

    return result;
}

int __is_valid_king_move(int xfrom, int yfrom, int xto, int yto, piece_type_t pieceType, config_t *config) {
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    if (xmove > 2 || ymove > 1) {
        return 0;
    } else if (xmove == 1) {
        return 1;
    } else if (xmove == 2) {
        piece_color_t color = piece_get_color(pieceType);
        if (!__castle_king_on_first_rank(yfrom, color)) {
            return 0;
        }
        return __castle_long(xfrom, xto) ? __can_long_castle(xfrom, xto, color, config) : __can_short_castle(xfrom, xto, color, config);
    }
    return 0;
}

bool __castle_long(int xfrom, int xto) {
    return xfrom > xto;
}

bool __castle_king_on_first_rank(int yfrom, piece_color_t color) {
    return color == BLACK ? yfrom == 0 : yfrom == 7;
}

int __can_short_castle(int xfrom, int xto, piece_color_t color, config_t *pConfig) {
    int y = color == WHITE ? 7 : 0;
    if (color == BLACK && pConfig->short_castles_black) {
        return __castle_clear_king_path_without_checks(xfrom, xto, 1, BLACK, pConfig, y, 3);
    } else if (color == WHITE && pConfig->short_castles_white) {
        return __castle_clear_king_path_without_checks(xfrom, xto, 1, WHITE, pConfig, y, 4);
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
    position_t *pos_to_check = position_new();
    position_set_x(pos_to_check, x);
    position_set_y(pos_to_check, y);
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_t *current_attacker = attackers[i];
        for (int ii = 0; ii < MAX_POSITIONS; ii++) {
            if (position_equal(pos_to_check, piece_get_available_position(current_attacker, ii))) {
                free(pos_to_check);
                return true;
            }
        }
    }
    free(pos_to_check);
    return false;
}

int __can_long_castle(int xfrom, int xto, piece_color_t color, config_t *pConfig) {
    int y = color == WHITE ? 7 : 0;
    if (pConfig->board[1][y] > NONE) {
        return 0;
    }
    if (color == BLACK && pConfig->long_castles_black) {
        return __castle_clear_king_path_without_checks(xfrom, xto, -1, BLACK, pConfig, y, 5);
    } else if (color == WHITE && pConfig->long_castles_white) {
        return __castle_clear_king_path_without_checks(xfrom, xto, -1, WHITE, pConfig, y, 6);
    }
    return 0;
}

int __is_providing_check(config_t *config, int xto, int yto) {
    piece_type_t type = config->board[xto][yto];

    return type == KING_W || type == KING_B;
}


int __is_valid_pawn_move(int xfrom, int yfrom, int xto, int yto, config_t *config) {
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);
    piece_type_t piece_at_to_position = config->board[xto][yto];
    piece_type_t piece_at_from_position = config->board[xfrom][yfrom];

    if (ymove == 2) {
        piece_color_t piece_color = piece_get_color(piece_at_from_position);
        int pawn_start_y = piece_color == WHITE ? 6 : 1;
        int y_direction = piece_color == WHITE ? -1 : 1;
        if (yfrom != pawn_start_y) {
            return 0;
        }
        if (config->board[xfrom][yfrom + y_direction] > NONE) {
            return 0;
        }
        return 2;
    }
    if (xmove > 1) {
        return 0;
    }
    if (xmove == 1 && piece_at_to_position == NONE) {
        return 0;
    }
    if (ymove == 1 && xmove == 0 && piece_at_to_position > NONE) {
        return 0;
    }
    if (piece_at_from_position == PAWN_W && yfrom - yto != 1) {
        return 0;
    }
    if (piece_at_from_position == PAWN_B && yfrom - yto != -1) {
        return 0;
    }
    return 1;
}

int __is_valid_knight_move(int xfrom, int yfrom, int xto, int yto) {
    int xmove = __abs(xfrom - xto);
    int ymove = __abs(yfrom - yto);

    if (xmove == 1 && ymove == 2)
        return 1;
    if (xmove == 2 && ymove == 1)
        return 1;
    return 0;
}

int __is_valid_bishop_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg) {
    int abs_xmove = __abs(xfrom - xto);
    int abs_ymove = __abs(yfrom - yto);

    int xmove = xfrom - xto;
    int ymove = yfrom - yto;

    if (abs_xmove == abs_ymove) {
        int y = ymove > 0 ? yfrom - 1 : yfrom + 1;
        int x = xmove > 0 ? xfrom - 1 : xfrom + 1;
        while (y != yto) {
            if (cfg->board[x][y] > NONE) {
                return 0;
            }
            ymove > 0 ? y-- : y++;
            xmove > 0 ? x-- : x++;
        }
        return 1;
    }
    return 0;
}

int __is_valid_rook_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg) {
    int xmove = xfrom - xto;
    int ymove = yfrom - yto;

    if (xmove == 0) {
        int y = ymove > 0 ? yfrom - 1 : yfrom + 1;
        while (y != yto) {
            if (cfg->board[xto][y] > NONE) {
                return 0;
            }
            ymove > 0 ? y-- : y++;
        }
        return 1;
    }
    if (ymove == 0) {
        int x = xmove > 0 ? xfrom - 1 : xfrom + 1;
        while (x != xto) {
            if (cfg->board[x][yto] > NONE) {
                return 0;
            }
            xmove > 0 ? x-- : x++;
        }
        return 1;
    }
    return 0;
}

int __is_valid_queen_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg) {
    return __is_valid_bishop_move(xfrom, yfrom, xto, yto, cfg) || __is_valid_rook_move(xfrom, yfrom, xto, yto, cfg);
}

int __abs(int x) {
    if (x < 0)
        return x * -1;
    return x;
}

int config_execute_move(config_t *conf, move_t *move) {
    position_t *from = move_get_from_position(move);
    position_t *to = move_get_to_position(move);

    config_t *backup_config = config_new();
    config_ctor(backup_config);
    config_copy(conf, backup_config);

    int move_executed = 0;
    conf->check_white = 0;
    conf->check_black = 0;

    if (from != NULL && to != NULL) {
        piece_color_t move_color = piece_get_color(move_get_piece_type(move));
        piece_t *piece = config_get_piece(conf, move_color, move_get_from_position(move));
        int valid_move = config_valid_move(conf, piece, position_get_x(to), position_get_y(to));
        if (valid_move) {
            int xfrom = position_get_x(from);
            conf->board[xfrom][position_get_y(from)] = NONE;

            int xto = position_get_x(to);
            int yto = position_get_y(to);
            if (conf->board[xto][yto] > NONE) {
                config_remove_piece(conf, to);
            } else if (conf->board[xto][yto] == EN_PASSANT && __is_pawn(piece)) {
                __move_en_passant(conf, piece, xto);
            }
            piece_type_t type = piece_get_type(piece);
            if (valid_move == 2) {
                int en_passant_rank = piece_get_color(type) == WHITE ? 5 : 2;
                conf->board[xto][en_passant_rank] = EN_PASSANT;
            } else {
                __clear_en_passant_flag(conf);
            }
            if (valid_move == 3 || valid_move == 4) {
                __castle_rook_move(conf, move_color, 7, 5);
            }
            if (valid_move == 5 || valid_move == 6) {
                __castle_rook_move(conf, move_color, 0, 3);
            }
            __config_update_castle_flags(conf, xfrom, &type);

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
    position_t *pPosition = position_new();

    position_set_x(pPosition, xfrom);
    position_set_y(pPosition, y);
    piece_t *pPiece = config_get_piece(pConfig, color, pPosition);

    position_t *rook_position = piece_get_current_position(pPiece);
    position_set_x(rook_position, xto);
    pConfig->board[xfrom][y] = NONE;
    pConfig->board[xto][y] = color == BLACK ? ROOK_B : ROOK_W;

    free(pPosition);
}

void __move_en_passant(config_t *conf, piece_t *piece, int xto) {
    int en_passant_piece_rank = piece_get_color(piece_get_type(piece)) == WHITE ? 3 : 4;
    position_t *en_passant_piece_position = position_new();
    position_set_x(en_passant_piece_position, xto);
    position_set_y(en_passant_piece_position, en_passant_piece_rank);
    config_remove_piece(conf, en_passant_piece_position);
    conf->board[xto][en_passant_piece_rank] = NONE;
    position_dtor(en_passant_piece_position);
    free(en_passant_piece_position);
}

bool __is_pawn(piece_t *piece) {
    piece_type_t type = piece_get_type(piece);
    return type == PAWN_B || type == PAWN_W;
}

void __clear_en_passant_flag(config_t *config) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        if (config->board[x][5] == EN_PASSANT) {
            config->board[x][5] = NONE;
        }
        if (config->board[x][2] == EN_PASSANT) {
            config->board[x][2] = NONE;
        }
    }
}

void config_remove_piece(config_t *cfg, position_t *position) {
    int x = position_get_x(position);
    int y = position_get_y(position);
    piece_type_t piece_type = cfg->board[x][y];
    piece_t **piece = NULL;
    if (piece_type < 7) {
        piece = cfg->white;
    } else {
        piece = cfg->black;
    }
    while (!position_equal(piece_get_current_position(*piece), position)) {
        piece++;
    }
    position_invalidate(piece_get_current_position(*piece));
    piece_set_type(*piece, NONE);

    for (int i = 0; i < MAX_POSITIONS; i++) {
        position_invalidate(piece_get_available_position(*piece, i));
    }
}

int config_move_cpu(config_t *conf) {
    printf("cpu move...\n");
    int piece_moved = 0;
    move_t *next_move = move_new();
    move_ctor(next_move);
    config_calculate_move(conf, next_move);

    if (move_get_piece_type(next_move) > NONE && position_valid(move_get_to_position(next_move))) {
        piece_moved = config_execute_move(conf, next_move);
    }
    config_print(conf);

    move_dtor(next_move);
    free(next_move);

    return piece_moved;
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
        move_dtor(best_path[i]);
        free(best_path[i]);

        move_dtor(current_path[i]);
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

piece_t *config_get_piece(config_t *config, piece_color_t color, position_t *position) {
    piece_t **pieces = color == WHITE ? config->white : config->black;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (position_equal(piece_get_current_position(pieces[i]), position)) {
            return pieces[i];
        }
    }
    return NULL;;
}

void config_copy(config_t *src, config_t *dst) {
    dst->check_white = src->check_white;
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
