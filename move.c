#include <stdlib.h>
#include <stdio.h>
#include "move.h"

struct move {
    piece_type_t piece_type;
    position_t *from_position;
    position_t *to_position;
    int score;
};

int __move_eval(move_t **path, int depth);

move_t *move_new() {
    return (move_t *) malloc(sizeof(move_t));
}

void move_ctor(move_t *move) {
    move->piece_type = NONE;
    move->from_position = position_new();
    position_ctor(move->from_position);
    move->to_position = position_new();
    position_ctor(move->to_position);
    move->score = -9999;
}

void move_dtor(move_t *move) {
    position_dtor(move->from_position);
    free(move->from_position);
    position_dtor(move->to_position);
    free(move->to_position);
}

void move_set_to_position(move_t *move, position_t *position) {
    position_copy(position, move->to_position);
}

void move_set_from_position(move_t *move, position_t *position) {
    position_copy(position, move->from_position);
}

void move_print(move_t **path, int size) {
    for (int i = 0; i < size; i++) {
        move_t *node = path[i];
        if (node->piece_type > NONE) {
            printf("(%c_%c:%c%d=%d) -> ", piece_type_to_char(node->piece_type), piece_get_color(node->piece_type) == WHITE ? 'W' : 'B',
                   position_get_x(node->to_position) + 'a', 8 - position_get_y(node->to_position),
                   node->score);
        }
    }
    printf("<END>\n");
}

void move_set_score(move_t *move, int score) {
    move->score = score;
}

int move_get_score(move_t *move) {
    return move->score;
}

position_t *move_get_to_position(move_t *move) {
    return move->to_position;
}

position_t *move_get_from_position(move_t *move) {
    return move->from_position;
}

void move_set_piece_type(move_t *move, piece_type_t type) {
    move->piece_type = type;
}

piece_type_t move_get_piece_type(move_t *move) {
    return move->piece_type;
}

int move_cmpr(move_t **a, move_t **b, int depth) {
    int a_eval = __move_eval(a, depth);
    int b_eval = __move_eval(b, depth);

   printf("curr (%d) > bst (%d) ? (%d)\n", a_eval, b_eval, a_eval > b_eval);

    return  a_eval > b_eval ;
}

int __move_eval(move_t **path, int depth) {
    int result = 0;
    for (int i = 0; i < depth; i++) {
       result += (depth - i) * move_get_score(path[i]);
    }
    return result;
}

void move_cpy(move_t **src, move_t **dst, int depth) {
    for(int i = 0; i < depth; i++) {
       move_t *src_node = src[i];
       move_t *dst_node = dst[i];

        move_set_piece_type(dst_node, move_get_piece_type(src_node));
        move_set_score(dst_node, move_get_score(src_node));
        move_set_from_position(dst_node, move_get_from_position(src_node));
        move_set_to_position(dst_node, move_get_to_position(src_node));
    }
}
