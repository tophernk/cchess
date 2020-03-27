#include <stdio.h>
#include "defintions.h"
#include "pieces.h"

typedef struct {
    int min;
    int max;
} score_t;

score_t execute_best_move(config_t *config, piece_color_t color_to_move, path_node_t *best_path, path_node_t *current_path, int current_depth);

path_node_t create_path_node();

int move_piece(move_t move, config_t *conf) {
    piece_t *piece = move.piece;
    pos_t to = move.to_position;
    if (piece != NULL) {
        for (int i = 0; i < sizeof(piece->available_positions) / sizeof(piece->available_positions[0]); i++) {
            pos_t available_pos = piece->available_positions[i];

            if (available_pos.x == to.x && available_pos.y == to.y) {
                conf->board[piece->current_position.x][piece->current_position.y] = NONE;

                if (conf->board[to.x][to.y] != NONE) {
                    remove_piece(to.x, to.y, conf);
                }

                piece->current_position.x = to.x;
                piece->current_position.y = to.y;
                conf->board[to.x][to.y] = piece->type;

                update_available_positions(conf);

                return evalConf(conf, BLACK);
            }
        }
    }
    return -9999;
}

void remove_piece(int x, int y, config_t *cfg) {
    piece_type_t piece_type = cfg->board[x][y];
    piece_t *piece = NULL;
    if (piece_type < 7) {
        piece = cfg->white;
    } else {
        piece = cfg->black;
    }
    while (piece->current_position.x != x || piece->current_position.y != y) {
        piece++;
    }
    invalidate_position(&piece->current_position);
    piece->type = NONE;
    for (int i = 0; i < MAX_POSITIONS; i++) {
        invalidate_position(&piece->available_positions[i]);
    }
}

int cpuMove(config_t *conf) {
    printf("cpu move...\n");
    int piece_moved = 0;
    move_t next_move = calculate_move(conf);
    if (next_move.piece->type != NONE && next_move.to_position.x != -1) {
        piece_moved = move_piece(next_move, conf);
    }
    printBoard(conf);
    return piece_moved;
}

move_t calculate_move(config_t *conf) {
    config_t tmp_conf = *conf;

    path_node_t best_path[DEPTH];
    path_node_t current_path[DEPTH];

    for (int i = 0; i < DEPTH; i++) {
        best_path[i] = create_path_node();
        current_path[i] = create_path_node();
    }

    for (int i = 0; i < DEPTH; i++) {
        piece_color_t turn_color = i % 2 == 0 ? BLACK : WHITE;
        execute_best_move(&tmp_conf, turn_color, best_path, current_path, i);
    }

    path_node_t move_to_play = best_path[0];

    move_t result;
    result.piece = &conf->black[move_to_play.piece_index];
    result.to_position = move_to_play.position;

    printf("-----------------\n");
    printf("best path: ");
    print_path(best_path, DEPTH);

    printf("=> best move: ");
    print_eval_move(result, best_path[DEPTH - 1].score);
    printf("\n");

    return result;
}

path_node_t create_path_node() {
    path_node_t node;
    node.piece = NONE;
    pos_t pos;
    pos.x = -1;
    pos.y = -1;
    node.position = pos;
    node.score = -9999;
    node.piece_index = 0;
    return node;
}

score_t execute_best_move(config_t *config, piece_color_t color_to_move, path_node_t *best_path, path_node_t *current_path, int current_depth) {
    int min_score = 9999;
    int max_score = -9999;
    int best_move_piece_index = 0;
    int best_move_position_index = 0;
    config_t tmp_conf = *config;

    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        piece_t *pieces = color_to_move == WHITE ? tmp_conf.white : tmp_conf.black;
        if (pieces[i].type != NONE) {
            for (int x = 0; x < MAX_POSITIONS; x++) {
                move_t move;
                move.piece = color_to_move == WHITE ? &tmp_conf.white[i] : &tmp_conf.black[i];
                move.to_position = pieces[i].available_positions[x];
                if (move.to_position.x != -1) {
                    printf("exec move: ");
                    int score = move_piece(move, &tmp_conf);
                    print_eval_move(move, score);
                    printf("\n");

                    path_node_t path_node = current_path[current_depth];
                    path_node.score = score;
                    path_node.piece = move.piece->type;
                    path_node.position = move.to_position;
                    path_node.piece_index = i;
                    current_path[current_depth] = path_node;

                    print_path(current_path, DEPTH);

                    if (score > max_score) {
                        max_score = score;
                        best_move_piece_index = i;
                        best_move_position_index = x;
                        best_path[current_depth] = path_node;
                    }

                    tmp_conf = (*config);
                }
            }
        }
    }
    piece_t *pieces = color_to_move == WHITE ? config->white : config->black;
    move_t best_move;
    best_move.piece = &pieces[best_move_piece_index];
    best_move.to_position = pieces[best_move_piece_index].available_positions[best_move_position_index];
    if (best_move.piece->type != NONE && best_move.to_position.x != -1) {
        move_piece(best_move, config);
    }
    score_t result;
    result.min = min_score;
    result.max = max_score;

    return result;
}

int is_white_piece(piece_type_t type) {
    switch (type) {
        case PAWN_W:
        case KNIGHT_W:
        case BISHOP_W:
        case ROOK_W:
        case QUEEN_W:
        case KING_W:
            return 1;
        default:
            return 0;
    }
}

int isValidMove(config_t *conf, int xfrom, int yfrom, int xto, int yto) {
    piece_type_t piece = conf->board[xfrom][yfrom];
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    piece_type_t piece_at_to_position = conf->board[xto][yto];
    if (piece_at_to_position != NONE) {
        if (is_white_piece(piece) && is_white_piece(piece_at_to_position))
            return 0;
        if (!is_white_piece(piece) && !is_white_piece(piece_at_to_position))
            return 0;
    }

    pos_t from;
    from.x = xfrom;
    from.y = yfrom;

    pos_t to;
    to.x = xto;
    to.y = yto;

    if (piece == PAWN_W || piece == PAWN_B) {
        return is_valid_pawn_move(xmove, ymove, yfrom, yto, piece, piece_at_to_position);
    } else if (piece == KNIGHT_W || piece == KNIGHT_B) {
        return is_valid_knight_move(from, to);
    } else if (piece == BISHOP_W || piece == BISHOP_B) {
        return is_valid_bishop_move(from, to, conf);
    } else if (piece == ROOK_W || piece == ROOK_B) {
        return is_valid_rook_move(from, to, conf);
    } else if (piece == KING_W || piece == KING_B) {
        if (xmove > 1 || ymove > 1)
            return 0;
        return 1;
    } else if (piece == QUEEN_W || piece == QUEEN_B) {
        return is_valid_queen_move(from, to, conf);
    }
    return 0;
}

int is_valid_pawn_move(int xmove, int ymove, int yfrom, int yto, piece_type_t piece, piece_type_t piece_at_to_position) {
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

int is_valid_knight_move(pos_t from, pos_t to) {
    int xmove = abs(from.x - to.x);
    int ymove = abs(from.y - to.y);

    if (xmove == 1 && ymove == 2)
        return 1;
    if (xmove == 2 && ymove == 1)
        return 1;
    return 0;
}

int is_valid_bishop_move(pos_t from, pos_t to, config_t *cfg) {
    int abs_xmove = abs(from.x - to.x);
    int abs_ymove = abs(from.y - to.y);

    int xmove = from.x - to.x;
    int ymove = from.y - to.y;

    if (abs_xmove == abs_ymove) {
        int y = ymove > 0 ? from.y - 1 : from.y + 1;
        int x = xmove > 0 ? from.x - 1 : from.x + 1;
        while (y != to.y) {
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

int is_valid_rook_move(pos_t from, pos_t to, config_t *cfg) {
    int xmove = from.x - to.x;
    int ymove = from.y - to.y;

    if (xmove == 0) {
        int y = ymove > 0 ? from.y - 1 : from.y + 1;
        while (y != to.y) {
            if (cfg->board[to.x][y] != NONE) {
                return 0;
            }
            ymove > 0 ? y-- : y++;
        }
        return 1;
    }
    if (ymove == 0) {
        int x = xmove > 0 ? from.x - 1 : from.x + 1;
        while (x != to.x) {
            if (cfg->board[x][to.y] != NONE) {
                return 0;
            }
            xmove > 0 ? x-- : x++;
        }
        return 1;
    }
    return 0;
}

int is_valid_queen_move(pos_t from, pos_t to, config_t *cfg) {
    return is_valid_bishop_move(from, to, cfg) || is_valid_rook_move(from, to, cfg);
}

