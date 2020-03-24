#include <stdio.h>
#include "defintions.h"
#include "pieces.h"

int movePiece(piece_t *p, int to_x, int to_y, config_t *conf) {
    if (p != NULL) {
        for (int i = 0; i < sizeof(p->available_positions) / sizeof(p->available_positions[0]); i++) {
            pos_t available_pos = p->available_positions[i];

            if (available_pos.x == to_x && available_pos.y == to_y) {
                conf->board[p->current_position.x][p->current_position.y] = NONE;

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

int cpuMove(config_t *conf) {
    move_t next_move = calculateMove(conf, 2);
    movePiece(next_move.p, next_move.to_position->x, next_move.to_position->y, conf);
    printf("cpu move...\n");
    printBoard(conf);
    return 1;
}

move_t calculateMove(config_t *conf, int depth) {
    config_t tmp = *conf;
    config_t backup = *conf;
    int best_piece_index;
    int best_move_index;
    int best_eval;

    for (int i = 0; i < depth; i++) {
        for (int x = 0; x < sizeof(tmp.black) / sizeof(tmp.black[0]); x++) {
            for (int y = 0;
                 y < sizeof(tmp.black[x].available_positions) / sizeof(tmp.black[x].available_positions[0]); y++) {
                pos_t to_position = tmp.black[x].available_positions[y];
                if (tmp.black[x].type != NONE && to_position.x != -1) {
                    movePiece(&tmp.black[x], to_position.x, to_position.y, &tmp);
                    int eval = evalConf(&tmp);
                    if (eval > best_eval) {
                        best_eval = eval;
                        best_piece_index = x;
                        best_move_index = y;
                    }
                    tmp = backup;

                    printf("print board from calculate move\n");
                    printBoard(conf);
                    printf("print tmp board from calculate move\n");
                    printBoard(&tmp);
                }
            }
        }
    }
    move_t result;
    result.p = &conf->black[best_piece_index];
    result.to_position = &conf->black[best_piece_index].available_positions[best_move_index];

    printf("best move: %d %d\n", best_piece_index, best_move_index);

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
    }
    return 0;
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
        if (xmove > 1) {
            result = 0;
        }
        if (piece == PAWN_W && yfrom - yto != 1) {
            result = 0;
        }
        if (piece == PAWN_B && yfrom - yto != -1) {
            result = 0;
        }
    } else if (piece == KNIGHT_W || piece == KNIGHT_B) {
        int xmove = abs(xfrom - xto);
        int ymove = abs(yfrom - yto);

        if (xmove == 1 && ymove == 2)
            return 1;
        if (xmove == 2 && ymove == 1)
            return 1;
        return 0;
    } else if (piece % 10 == 3) {
        return isValidBishopMove(xmove, ymove);
    } else if (piece % 10 == 5) {
        return isValidRookMove(xmove, ymove);
    } else if (piece % 10 == 6) {
        if (xmove > 1 || ymove > 1)
            return 0;
        return 1;
    } else if (piece % 10 == 9) {
        return isValidQueenMove(xmove, ymove);
    } else {
        result = 0;
    }
    return result;
}

int isValidBishopMove(int xmove, int ymove) {
    if (xmove != ymove)
        return 0;
    return 1;
}

int isValidRookMove(int xmove, int ymove) {
    if (xmove == 0 || ymove == 0)
        return 1;
    return 0;
}

int isValidQueenMove(int xmove, int ymove) {
    return isValidBishopMove(xmove, ymove) || isValidRookMove(xmove, ymove);
}

