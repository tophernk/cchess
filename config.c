#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "cchess.h"
#include "config_p.h"
#include "path_node.h"

int __is_valid_pawn_move(int xmove, int ymove, int yfrom, int yto, piece_type_t piece, piece_type_t piece_at_to_position);

int __is_valid_knight_move(int xfrom, int yfrom, int xto, int yto);

int __is_valid_rook_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_bishop_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_queen_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

void __config_add_piece(config_t *, piece_type_t, int x, int y, piece_color_t, int index);

void __determine_available_positions(piece_t *, config_t *);

int __abs(int);

void __execute_all_moves(config_t *config, piece_color_t color_to_move, path_node_t **best_path, path_node_t **current_path, int current_depth);

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

    __config_add_piece(config, PAWN_W, 1, 6, WHITE, 0);
    __config_add_piece(config, BISHOP_W, 2, 7, WHITE, 1);
    __config_add_piece(config, PAWN_B, 0, 1, BLACK, 0);
    __config_add_piece(config, KNIGHT_B, 1, 0, BLACK, 1);

    config_update_available_positions(config);
}

void config_dtor(config_t *config) {
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_dtor(config->white[i]);
        free(config->white[i]);

        piece_dtor(config->black[i]);
        free(config->black[i]);
    }
}

void __config_add_piece(config_t *config, piece_type_t type, int x, int y, piece_color_t color, int index) {
    config->board[x][y] = type;

    piece_t *piece = color == BLACK ? config->black[index] : config->white[index];
    piece_set_type(piece, type);
    piece_set_current_position(piece, x, y);
    piece_invalidate_available_positions(piece);
}

void config_update_available_positions(config_t *conf) {
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (piece_get_type(conf->white[i]) != NONE)
            __determine_available_positions(conf->white[i], conf);
    }
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (piece_get_type(conf->black[i]) != NONE)
            __determine_available_positions(conf->black[i], conf);
    }
}


int config_eval(config_t *config, piece_color_t color) {
    int white = 0;
    int black = 0;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        white += piece_get_type(config->white[i]);
        black += piece_get_type(config->black[i]);
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

void __execute_all_moves(config_t *config, piece_color_t color_to_move, path_node_t **best_path, path_node_t **current_path, int current_depth) {
    if (current_depth == DEPTH) {
        path_node_print(current_path, DEPTH);
        // eval and compare current path to best path
        return;
    }
    int max_score = -9999;
    config_t *tmp_conf = config_new();
    config_copy(config, tmp_conf);
    path_node_t *move = path_node_new();
    path_node_ctor(move);

    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_t **pieces = color_to_move == WHITE ? tmp_conf->white : tmp_conf->black;
        if (piece_get_type(pieces[i]) != NONE) {
            for (int x = 0; x < MAX_POSITIONS; x++) {
                piece_t *piece_to_move = color_to_move == WHITE ? tmp_conf->white[i] : tmp_conf->black[i];
                position_t *available_position = piece_get_available_position(piece_to_move, x);
                if (position_get_x(available_position) != -1) {
                    path_node_set_piece_type(move, piece_get_type(piece_to_move));
                    path_node_set_from_position(move, piece_get_current_position(piece_to_move));
                    path_node_set_to_position(move, available_position);
                    int score = config_execute_move(tmp_conf, move);

                    path_node_t *current_path_node = current_path[current_depth];
                    path_node_ctor(current_path_node);
                    path_node_set_piece_type(current_path_node, piece_get_type(piece_to_move));
                    path_node_set_from_position(current_path_node, path_node_get_from_position(move));
                    path_node_set_to_position(current_path_node, path_node_get_to_position(move));
                    path_node_set_score(current_path_node, score);

                    if (score > max_score) {
                        max_score = score;
                        path_node_t *best_path_node = best_path[current_depth];
                        path_node_ctor(best_path_node);
                        path_node_set_piece_type(best_path_node, piece_get_type(piece_to_move));
                        path_node_set_from_position(best_path_node, path_node_get_from_position(move));
                        path_node_set_to_position(best_path_node, path_node_get_to_position(move));
                        path_node_set_score(best_path_node, score);
                    }

                    __execute_all_moves(tmp_conf, color_to_move == WHITE ? BLACK : WHITE, best_path, current_path, current_depth + 1);

                    config_copy(config, tmp_conf);
                }
            }
        }
    }

    path_node_dtor(move);
    free(move);

    config_dtor(tmp_conf);
    free(tmp_conf);
}

int config_valid_move(config_t *conf, piece_t *piece, int xto, int yto) {
    piece_type_t piece_at_from_position = piece_get_type(piece);
    position_t *from_position = piece_get_current_position(piece);
    int xfrom = position_get_x(from_position);
    int yfrom = position_get_y(from_position);
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    piece_type_t piece_at_to_position = conf->board[xto][yto];
    if (piece_at_to_position != NONE) {
        if (piece_get_color(piece_at_from_position) && piece_get_color(piece_at_to_position)) {
            return 0;
        }
    }

    if (piece_at_from_position == PAWN_W || piece_at_from_position == PAWN_B) {
        return __is_valid_pawn_move(xmove, ymove, yfrom, yto, piece_at_from_position, piece_at_to_position);
    } else if (piece_at_from_position == KNIGHT_W || piece_at_from_position == KNIGHT_B) {
        return __is_valid_knight_move(xfrom, yfrom, xto, yto);
    } else if (piece_at_from_position == BISHOP_W || piece_at_from_position == BISHOP_B) {
        return __is_valid_bishop_move(xfrom, yfrom, xto, yto, conf);
    } else if (piece_at_from_position == ROOK_W || piece_at_from_position == ROOK_B) {
        return __is_valid_rook_move(xfrom, yfrom, xto, yto, conf);
    } else if (piece_at_from_position == KING_W || piece_at_from_position == KING_B) {
        if (xmove > 1 || ymove > 1)
            return 0;
        return 1;
    } else if (piece_at_from_position == QUEEN_W || piece_at_from_position == QUEEN_B) {
        return __is_valid_queen_move(xfrom, yfrom, xto, yto, conf);
    }
    return 0;
}

int __is_valid_pawn_move(int xmove, int ymove, int yfrom, int yto, piece_type_t piece, piece_type_t piece_at_to_position) {
    if (xmove > 1) {
        return 0;
    }
    if (xmove == 1 && piece_at_to_position == NONE) {
        return 0;
    }
    if (ymove == 1 && xmove == 0 && piece_at_to_position != NONE) {
        return 0;
    }
    if (piece == PAWN_W && yfrom - yto != 1) {
        return 0;
    }
    if (piece == PAWN_B && yfrom - yto != -1) {
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
            if (cfg->board[x][y] != NONE) {
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
            if (cfg->board[xto][y] != NONE) {
                return 0;
            }
            ymove > 0 ? y-- : y++;
        }
        return 1;
    }
    if (ymove == 0) {
        int x = xmove > 0 ? xfrom - 1 : xfrom + 1;
        while (x != xto) {
            if (cfg->board[x][yto] != NONE) {
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

int config_execute_move(config_t *conf, path_node_t *move) {
    position_t *from = path_node_get_from_position(move);
    position_t *to = path_node_get_to_position(move);
    if (from != NULL && to != NULL) {
        piece_t *piece = config_get_piece(conf, piece_get_color(path_node_get_piece_type(move)), path_node_get_from_position(move));
        if (config_valid_move(conf, piece, position_get_x(to), position_get_y(to))) {
            conf->board[position_get_x(from)][position_get_y(from)] = NONE;

            int xto = position_get_x(to);
            int yto = position_get_y(to);
            if (conf->board[xto][yto] != NONE) {
                config_remove_piece(conf, to);
            }

            piece_set_current_position(piece, xto, yto);
            conf->board[xto][yto] = piece_get_type(piece);

            config_update_available_positions(conf);

            return config_eval(conf, BLACK);
        }
    }
    return -9999;
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
    path_node_t *next_move = path_node_new();
    path_node_ctor(next_move);
    config_calculate_move(conf, next_move);

    if (path_node_get_piece_type(next_move) != NONE && position_valid(path_node_get_to_position(next_move))) {
        piece_moved = config_execute_move(conf, next_move);
    }
    config_print(conf);

    path_node_dtor(next_move);
    free(next_move);

    return piece_moved;
}

void config_calculate_move(config_t *conf, path_node_t *calculated_move) {
    config_t *tmp_conf = config_new();
    config_copy(conf, tmp_conf);

    path_node_t *best_path[DEPTH];
    path_node_t *current_path[DEPTH];

    for (int i = 0; i < DEPTH; i++) {
        best_path[i] = path_node_new();
        path_node_ctor(best_path[i]);

        current_path[i] = path_node_new();
        path_node_ctor(current_path[i]);
    }

    for (int i = 0; i < DEPTH; i++) {
        piece_color_t turn_color = i % 2 == 0 ? BLACK : WHITE;
        __execute_all_moves(tmp_conf, turn_color, best_path, current_path, i);
    }

    path_node_t *node_to_play = best_path[0];
    piece_t *piece_to_move = config_get_piece(conf, BLACK, path_node_get_from_position(node_to_play));
    path_node_set_piece_type(calculated_move, piece_get_type(piece_to_move));
    path_node_set_from_position(calculated_move, path_node_get_from_position(node_to_play));
    path_node_set_to_position(calculated_move, path_node_get_to_position(node_to_play));
    path_node_set_score(calculated_move, path_node_get_score(node_to_play));

    printf("-----------------\n");
    printf("best path: ");
    path_node_print(best_path, DEPTH);

    for (int i = 0; i < DEPTH; i++) {
        path_node_dtor(best_path[i]);
        free(best_path[i]);

        path_node_dtor(current_path[i]);
        free(current_path[i]);
    }
}

int config_move_available(config_t *config, piece_color_t color) {
    piece_t **piece = color == WHITE ? config->white : config->black;
    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        if (piece_get_type(piece[i]) != NONE) {
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
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            dst->board[x][y] = src->board[x][y];
        }

        for (int i = 0; i < NUMBER_OF_PIECES; i++) {
            dst->white[i] = piece_new();
            piece_copy(src->white[i], dst->white[i]);
            dst->black[i] = piece_new();
            piece_copy(src->black[i], dst->black[i]);
        }
    }
}
