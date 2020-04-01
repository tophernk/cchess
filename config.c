#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "cchess.h"
#include "config_p.h"
#include "path_node.h"

typedef struct {
    int min;
    int max;
} score_t;

int __is_valid_pawn_move(int xmove, int ymove, int yfrom, int yto, piece_type_t piece, piece_type_t piece_at_to_position);

int __is_valid_knight_move(int xfrom, int yfrom, int xto, int yto);

int __is_valid_rook_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_bishop_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

int __is_valid_queen_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg);

void __config_add_piece(config_t *, piece_type_t, int x, int y, piece_color_t, int index);

void __determine_available_positions(piece_t *, config_t *);

int __abs(int);

score_t __execute_best_move(config_t *config, piece_color_t color_to_move, path_node_t **best_path, path_node_t **current_path, int current_depth);

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
    __config_add_piece(config, PAWN_B, 0, 1, BLACK, 0);

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

score_t __execute_best_move(config_t *config, piece_color_t color_to_move, path_node_t **best_path, path_node_t **current_path, int current_depth) {
    int min_score = 9999;
    int max_score = -9999;
    int best_move_piece_index = 0;
    int best_move_position_index = 0;
    config_t tmp_conf = *config;
    move_t *move = move_new();
    move_ctor(move);

    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_t **pieces = color_to_move == WHITE ? tmp_conf.white : tmp_conf.black;
        if (piece_get_type(pieces[i]) != NONE) {
            for (int x = 0; x < MAX_POSITIONS; x++) {
                piece_t *piece_to_move = color_to_move == WHITE ? tmp_conf.white[i] : tmp_conf.black[i];
                position_t *available_position = piece_get_available_position(piece_to_move, x);
                if (position_get_x(available_position) != -1) {
                    move_set_piece(move, piece_to_move);
                    move_set_to_position(move, available_position);
                    printf("exec move: ");
                    int score = config_execute_move(&tmp_conf, move);
                    move_print(move, score);
                    printf("\n");

                    path_node_t *node = current_path[current_depth];
                    piece_t *piece_of_actual_config = color_to_move == WHITE ? config->white[i] : config->black[i];
                    path_node_ctor(node, piece_of_actual_config, piece_get_available_position(piece_of_actual_config, x));
                    path_node_set_score(node, score);
                    path_node_print(current_path, DEPTH);

                    if (score > max_score) {
                        max_score = score;
                        best_move_piece_index = i;
                        best_move_position_index = x;
                        best_path[current_depth] = node;
                    }

                    tmp_conf = (*config);
                }
            }
        }
    }

    piece_t **pieces = color_to_move == WHITE ? config->white : config->black;
    piece_t *best_piece = pieces[best_move_piece_index];
    move_set_piece(move, best_piece);
    position_t *best_position = piece_get_available_position(best_piece, best_move_position_index);
    move_set_to_position(move, best_position);

    if (piece_get_type(best_piece) != NONE && position_get_x(best_position) != -1) {
        config_execute_move(config, move);
    }

    score_t result;
    result.min = min_score;
    result.max = max_score;

    move_dtor(move);
    free(move);

    return result;
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
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    if (xmove == 1 && ymove == 2)
        return 1;
    if (xmove == 2 && ymove == 1)
        return 1;
    return 0;
}

int __is_valid_bishop_move(int xfrom, int yfrom, int xto, int yto, config_t *cfg) {
    int abs_xmove = abs(xfrom - xto);
    int abs_ymove = abs(yfrom - yto);

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

int config_execute_move(config_t *conf, move_t *move) {
    piece_t *piece = move_get_piece(move);
    position_t *to = move_get_to_position(move);
    if (piece != NULL) {
        for (int i = 0; i < MAX_POSITIONS; i++) {
            position_t *available_pos = piece_get_available_position(piece, i);

            if (position_equal(available_pos, to)) {
                position_t *current_position = piece_get_current_position(piece);
                conf->board[position_get_x(current_position)][position_get_y(current_position)] = NONE;

                if (conf->board[position_get_x(to)][position_get_y(to)] != NONE) {
                    config_remove_piece(conf, to);
                }

                position_set_x(to, position_get_x(to));
                position_set_y(to, position_get_y(to));
                conf->board[position_get_x(to)][position_get_y(to)] = piece_get_type(piece);

                config_update_available_positions(conf);

                return config_eval(conf, BLACK);
            }
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
    position_t *current_position = piece_get_current_position(*piece);
    while (!position_equal(current_position, position)) {
        piece++;
    }
    position_invalidate(current_position);
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

    if (piece_get_type(move_get_piece(next_move)) != NONE && position_valid(move_get_to_position(next_move))) {
        piece_moved = config_execute_move(conf, next_move);
    }
    config_print(conf);

    move_dtor(next_move);
    free(next_move);

    return piece_moved;
}

void config_calculate_move(config_t *conf, move_t *calculated_move) {
    config_t tmp_conf = *conf;

    path_node_t *best_path[DEPTH];
    path_node_t *current_path[DEPTH];

    for (int i = 0; i < DEPTH; i++) {
        best_path[i] = path_node_new();
        current_path[i] = path_node_new();
    }

    for (int i = 0; i < DEPTH; i++) {
        piece_color_t turn_color = i % 2 == 0 ? BLACK : WHITE;
        __execute_best_move(&tmp_conf, turn_color, best_path, current_path, i);
    }

    path_node_t *node_to_play = best_path[0];
    move_set_piece(calculated_move, path_node_get_piece(node_to_play));
    move_set_to_position(calculated_move, path_node_get_position(node_to_play));

    printf("-----------------\n");
    printf("best path: ");
    path_node_print(best_path, DEPTH);

    printf("=> best move: ");
    move_print(calculated_move, path_node_get_score(best_path[DEPTH - 1]));
    printf("\n");

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
            return 1;
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
