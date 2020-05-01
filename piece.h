#ifndef CCHESS_PIECE_H
#define CCHESS_PIECE_H

#include "position.h"

#define MAX_POSITIONS 27

typedef struct piece piece_t;

typedef enum {
    NONE,
    PAWN_W,
    KNIGHT_W,
    BISHOP_W,
    ROOK_W,
    KING_W,
    QUEEN_W,
    PAWN_B,
    KNIGHT_B,
    BISHOP_B,
    ROOK_B,
    KING_B,
    QUEEN_B
} piece_type_t;

typedef enum {
    BLACK,
    WHITE
} piece_color_t;

piece_t *piece_new();

void piece_ctor(piece_t *);

void piece_dtor(piece_t *);

void piece_copy(piece_t *src, piece_t *dst);

piece_color_t piece_get_color(piece_type_t);

piece_type_t piece_get_type(piece_t *);

void piece_set_type(piece_t *, piece_type_t);

char piece_type_to_char(piece_type_t);

piece_type_t piece_char_to_type(char);

int piece_type_get_weight(piece_type_t);

void piece_invalidate_available_positions(piece_t *);

void piece_set_current_position(piece_t *, int x, int y);

position_t *piece_get_current_position(piece_t *);

void piece_set_available_position(piece_t *, int x, int y, int index);

position_t *piece_get_available_position(piece_t *, int);

#endif //CCHESS_CONFIG_H
