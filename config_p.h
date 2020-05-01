#ifndef CCHESS_CONFIG_P_H
#define CCHESS_CONFIG_P_H
#include <stdbool.h>
#include "cchess.h"

struct config {
    piece_type_t board[BOARD_SIZE][BOARD_SIZE];
    piece_t *white[NUMBER_OF_PIECES];
    piece_t *black[NUMBER_OF_PIECES];
    bool check_white;
    bool check_black;
    bool short_castles_white;
    bool long_castles_white;
    bool short_castles_black;
    bool long_castles_black;
    char enpassant[2];
};

#endif //CCHESS_CONFIG_P_H
