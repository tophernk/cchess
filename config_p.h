#ifndef CCHESS_CONFIG_P_H
#define CCHESS_CONFIG_P_H

#include "cchess.h"

struct config {
    piece_type_t board[BOARD_SIZE][BOARD_SIZE];
    piece_t *white[NUMBER_OF_PIECES];
    piece_t *black[NUMBER_OF_PIECES];
    int check_white;
    int check_black;
};

#endif //CCHESS_CONFIG_P_H
