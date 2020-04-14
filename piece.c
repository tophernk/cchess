#include <stdlib.h>
#include "piece.h"

struct piece {
    piece_type_t type;
    position_t *current_position;
    position_t *available_positions[MAX_POSITIONS];
};

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

piece_t *piece_new() {
    return (piece_t *) malloc(sizeof(piece_t));
}

void piece_ctor(piece_t *piece) {
    piece->type = NONE;
    piece->current_position = position_new();
    position_ctor(piece->current_position);
    for (int i = 0; i < MAX_POSITIONS; i++) {
        piece->available_positions[i] = position_new();
        position_ctor(piece->available_positions[i]);
    }
}

void piece_dtor(piece_t *piece) {
    position_dtor(piece->current_position);
    free(piece->current_position);
    for (int i = 0; i < MAX_POSITIONS; i++) {
        position_dtor(piece->available_positions[i]);
        free(piece->available_positions[i]);
    }
}

piece_color_t piece_get_color(piece_type_t type) {
    return is_white_piece(type) ? WHITE : BLACK;
}

piece_type_t piece_get_type(piece_t *piece) {
    return piece->type;
}

char piece_type_to_char(piece_type_t piece) {
    switch (piece) {
        case PAWN_W:
        case PAWN_B:
            return 'P';
        case KNIGHT_W:
        case KNIGHT_B:
            return 'N';
        case BISHOP_W:
        case BISHOP_B:
            return 'B';
        case ROOK_W:
        case ROOK_B:
            return 'R';
        case QUEEN_W:
        case QUEEN_B:
            return 'Q';
        case KING_W:
        case KING_B:
            return 'K';
        default:
            return ' ';
    }
}

void piece_invalidate_available_positions(piece_t *piece) {
    for (int i = 0; i < MAX_POSITIONS; i++) {
        position_invalidate(piece->available_positions[i]);
    }
}

position_t *piece_get_current_position(piece_t *piece) {
    return piece->current_position;
}

void piece_set_available_position(piece_t *piece, int x, int y, int index) {
    position_set_x(piece->available_positions[index], x);
    position_set_y(piece->available_positions[index], y);
}

position_t *piece_get_available_position(piece_t *piece, int index) {
    return piece->available_positions[index];
}

void piece_set_type(piece_t *piece, piece_type_t type) {
    piece->type = type;
}

void piece_set_current_position(piece_t *piece, int x, int y) {
    position_set_x(piece->current_position, x);
    position_set_y(piece->current_position, y);
}

void piece_copy(piece_t *src, piece_t *dst) {
    dst->type = src->type;
    dst->current_position = position_new();
    position_copy(src->current_position, dst->current_position);
    for(int i = 0; i < MAX_POSITIONS; i++) {
        dst->available_positions[i] = position_new();
        position_copy(src->available_positions[i], dst->available_positions[i]);
    }
}
