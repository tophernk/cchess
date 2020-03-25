#include <stdio.h>
#include "defintions.h"
#include "pieces.h"

int movePiece(piece_t *p, int to_x, int to_y, config_t *conf) {
    if (p != NULL) {
        for (int i = 0; i < sizeof(p->available_positions) / sizeof(p->available_positions[0]); i++) {
            pos_t available_pos = p->available_positions[i];

            if (available_pos.x == to_x && available_pos.y == to_y) {
                conf->board[p->current_position.x][p->current_position.y] = NONE;

                if (conf->board[to_x][to_y] != NONE) {
                    remove_piece(to_x, to_y, conf);
                }

                p->current_position.x = to_x;
                p->current_position.y = to_y;
                conf->board[to_x][to_y] = p->type;

                update_available_positions(conf);
                return 1;
            }
        }
    }
    return 0;
}

void remove_piece(int x, int y, config_t *cfg) {
    piece_type_t piece_type = cfg->board[x][y];
    piece_t *piece = NULL;
    if (piece_type < 7) {
        piece = cfg->white;
    } else {
        piece = cfg->black;
    }
    while (piece->current_position.x != x && piece->current_position.y != y) {
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
    move_t next_move = calculateMove(conf, 2);
    if (next_move.p->type != NONE) {
        piece_moved = movePiece(next_move.p, next_move.to_position->x, next_move.to_position->y, conf);
    }
    printBoard(conf);
    return piece_moved;
}

move_t calculateMove(config_t *conf, int depth) {
    config_t tmp = *conf;
    config_t backup = *conf;
    int best_piece_index = 0;
    int best_move_index = 0;
    int best_eval = -1000;

    for (int i = 0; i < depth; i++) {
        for (int x = 0; x < sizeof(tmp.black) / sizeof(tmp.black[0]); x++) {
            if (tmp.black[x].type != NONE) {
                for (int y = 0; y < sizeof(tmp.black[x].available_positions) / sizeof(tmp.black[x].available_positions[0]); y++) {
                    pos_t to_position = tmp.black[x].available_positions[y];
                    if (to_position.x != -1) {
                        movePiece(&tmp.black[x], to_position.x, to_position.y, &tmp);
                        int eval = evalConf(&tmp);
                        printf("eval: %d ", eval);
                        if (eval > best_eval) {
                            best_eval = eval;
                            best_piece_index = x;
                            best_move_index = y;
                        }
                        tmp = backup;
                    }
                }
            }
        }
    }
    printf("best eval: %d\n", best_eval);
    move_t result;
    result.p = &conf->black[best_piece_index];
    result.to_position = &conf->black[best_piece_index].available_positions[best_move_index];

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
    int result = 1;
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

    if (piece == PAWN_W || piece == PAWN_B) {
        return is_valid_pawn_move(xmove, ymove, yfrom, yto, piece, piece_at_to_position);
    } else if (piece == KNIGHT_W || piece == KNIGHT_B) {
        return is_valid_knight_move(xfrom, xto, yfrom, yto);
    } else if (piece == BISHOP_W || piece == BISHOP_B) {
        return is_valid_bishop_move(xmove, ymove);
    } else if (piece == ROOK_W || piece == ROOK_B) {
        return is_valid_rook_move(xmove, ymove);
    } else if (piece == KING_W || piece == KING_B) {
        if (xmove > 1 || ymove > 1)
            return 0;
        return 1;
    } else if (piece == QUEEN_W || piece == QUEEN_B) {
        return is_valid_queen_move(xmove, ymove);
    } else {
        result = 0;
    }
    return result;
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

int is_valid_knight_move(int xfrom, int xto, int yfrom, int yto) {
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    if (xmove == 1 && ymove == 2)
        return 1;
    if (xmove == 2 && ymove == 1)
        return 1;
    return 0;
}

int is_valid_bishop_move(int xmove, int ymove) {
    if (xmove != ymove)
        return 0;
    return 1;
}

int is_valid_rook_move(int xmove, int ymove) {
    if (xmove == 0 || ymove == 0)
        return 1;
    return 0;
}

int is_valid_queen_move(int xmove, int ymove) {
    return is_valid_bishop_move(xmove, ymove) || is_valid_rook_move(xmove, ymove);
}

