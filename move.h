#ifndef CCHESS_MOVE_H
#define CCHESS_MOVE_H

#include "piece.h"

typedef struct move move_t;

move_t *move_new();

void move_ctor(move_t *);

void move_dtor(move_t *);

void move_print(move_t *move, int score);

void move_set_to_position(move_t *, position_t *);

void move_set_piece(move_t *, piece_t *);

piece_t *move_get_piece(move_t *);

position_t *move_get_to_position(move_t *);

#endif //CCHESS_MOVE_H
