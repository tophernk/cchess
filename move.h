#ifndef CCHESS_MOVE_H
#define CCHESS_MOVE_H

#include "piece.h"

typedef struct move move_t;

move_t *move_new();

void move_ctor(move_t *);

void move_dtor(move_t *);

position_t *move_get_to_position(move_t *);

char *move_get_from_position(move_t *);

void move_set_score(move_t *move, int score);

void move_set_from_position(move_t *move, char *);

void move_set_to_position(move_t *move, position_t *position);

int move_get_score(move_t *move);

void move_set_piece_type(move_t *move, piece_type_t type);

piece_type_t move_get_piece_type(move_t *move);

void move_print(move_t **path, int size);

int move_cmpr(move_t **a, move_t **b, int depth);

void move_cpy(move_t **src, move_t **dst, int depth);

#endif //CCHESS_MOVE_H
