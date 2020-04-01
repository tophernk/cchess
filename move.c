#include <stdlib.h>
#include <stdio.h>
#include "move.h"

struct move {
    piece_t *piece;
    position_t *to_position;
};

move_t *move_new() {
    return (move_t *) malloc(sizeof(move_t));
}

void move_ctor(move_t *move) {
    move->piece = piece_new();
    move->to_position = position_new();
}

void move_dtor(move_t *move) {
    piece_dtor(move->piece);
    position_dtor(move->to_position);
}

void move_print(move_t *move, int score) {
    printf("%c:%c%d=%d ", piece_tpye_to_char(piece_get_type(move->piece)), position_get_x(move->to_position) + 'a', 8 - position_get_y(move->to_position),
           score);
}

void move_set_to_position(move_t *move, position_t *position) {
    move->to_position = position;
}

void move_set_piece(move_t *move, piece_t *piece) {
    move->piece = piece;
}

piece_t *move_get_piece(move_t *move) {
    return move->piece;
}

position_t *move_get_to_position(move_t *move) {
    return move->to_position;
}
