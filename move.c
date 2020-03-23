#include <stdio.h>
#include "defintions.h"
#include "pieces.h"

int movePiece(piece_t *p, pos_t *to, config_t *conf) {
    if (p != NULL) {
        for (int i = 0; i < sizeof(p->available_positions) / sizeof(p->available_positions[0]); i++) {
            if (&p->available_positions[i] == to) {
                p->current_position = *to;

                update_available_positions(conf);
                return 1;
            }
        }
    }
    return 0;
}

int cpuMove(config_t *conf) {
    move_t next_move = calculateMove(conf, 2);
    movePiece(next_move.p, next_move.to_position, conf);
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
                if (tmp.black[x].type != 0 && &tmp.black[x].available_positions[y] != 0) {
                    movePiece(&tmp.black[x], &tmp.black[x].available_positions[y], &tmp);
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

int isValidMove(config_t *conf, int xfrom, int yfrom, int xto, int yto) {
    int result = 1;
    int piece = conf->board[yfrom][xfrom];
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    int to_pos = conf->board[yto][xto];
    if (to_pos != 0) {
        if (piece < 10 && to_pos < 10)
            return 0;
        if (piece > 10 && to_pos > 10)
            return 0;
    }

    if (piece % 10 == 1) {
        if (xmove > 1) {
            result = 0;
        }
        if (xmove == 1 && conf->board[yto][xto] == 0) {
            result = 0;
        }
        if (piece == PAWN_W && yfrom - yto != 1) {
            result = 0;
        }
        if (piece == PAWN_B && yfrom - yto != -1) {
            result = 0;
        }
    } else if (piece % 10 == 2) {
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
        if (xmove == 0 && ymove && 0)
            return 0;
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

