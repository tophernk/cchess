#include <stdlib.h>
#include "piece.h"

struct piece {
    piece_type_t type;
    position_t *available_positions[MAX_POSITIONS];
    char current_pos[2];
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
    for (int i = 0; i < MAX_POSITIONS; i++) {
        piece->available_positions[i] = position_new();
        position_ctor(piece->available_positions[i]);
    }
    position_init(piece->current_pos);
}

void piece_dtor(piece_t *piece) {
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

int piece_type_get_weight(piece_type_t type) {
    switch (type) {
        case PAWN_W:
        case PAWN_B:
            return 1;
        case KNIGHT_W:
        case KNIGHT_B:
        case BISHOP_W:
        case BISHOP_B:
            return 2;
        case ROOK_W:
        case ROOK_B:
            return 5;
        case QUEEN_W:
        case QUEEN_B:
            return 9;
        case KING_W:
        case KING_B:
            return 20;
        default:
            return 0;
    }
}

char piece_type_to_char(piece_type_t piece) {
    switch (piece) {
        case PAWN_W:
            return 'P';
        case PAWN_B:
            return 'p';
        case KNIGHT_W:
            return 'N';
        case KNIGHT_B:
            return 'n';
        case BISHOP_W:
            return 'B';
        case BISHOP_B:
            return 'b';
        case ROOK_W:
            return 'R';
        case ROOK_B:
            return 'r';
        case QUEEN_W:
            return 'Q';
        case QUEEN_B:
            return 'q';
        case KING_W:
            return 'K';
        case KING_B:
            return 'k';
        default:
            return ' ';
    }
}

piece_type_t piece_char_to_type(char fenv) {
    switch (fenv) {
        case 'P':
            return PAWN_W;
        case 'p':
            return PAWN_B;
        case 'N':
            return KNIGHT_W;
        case 'n':
            return KNIGHT_B;
        case 'B':
            return BISHOP_W;
        case 'b':
            return BISHOP_B;
        case 'R':
            return ROOK_W;
        case 'r':
            return ROOK_B;
        case 'Q':
            return QUEEN_W;
        case 'q':
            return QUEEN_B;
        case 'K':
            return KING_W;
        case 'k':
            return KING_B;
        default:
            return NONE;
    }
}

void piece_invalidate_available_positions(piece_t *piece) {
    for (int i = 0; i < MAX_POSITIONS; i++) {
        position_invalidate(piece->available_positions[i]);
    }
}

char *piece_get_current_position(piece_t *piece) {
    return piece->current_pos;
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
    piece->current_pos[0] = position_get_file(x);
    piece->current_pos[1] = position_get_rank(y);
}

void piece_copy(piece_t *src, piece_t *dst) {
    dst->type = src->type;
    for (int i = 0; i < MAX_POSITIONS; i++) {
        position_copy(src->available_positions[i], dst->available_positions[i]);
    }
    dst->current_pos[0] = src->current_pos[0];
    dst->current_pos[1] = src->current_pos[1];
}
