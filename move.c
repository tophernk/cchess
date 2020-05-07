#include <stdlib.h>
#include "move.h"
#include "logger.h"

struct move {
    char from[2];
    char to[2];
    piece_type_t piece_type;
    int score;
};

int __move_eval(move_t **path, int depth);

move_t *move_new() {
    return (move_t *) malloc(sizeof(move_t));
}

void move_ctor(move_t *move) {
    move->piece_type = NONE;
    position_invalidate(move->from);
    position_invalidate(move->to);
    move->score = -9999;
}

void move_set_to_position(move_t *move, const char *position) {
    move->to[0] = position[0];
    move->to[1] = position[1];
}

void move_set_from_position(move_t *move, const char *position) {
    move->from[0] = position[0];
    move->from[1] = position[1];
}

void move_print(move_t **path, int size) {
    for (int i = 0; i < size; i++) {
        move_t *node = path[i];
        if (node->piece_type > NONE) {
            cchess_log("(%c_%c:%c%d=%d) -> ", piece_type_to_char(node->piece_type), piece_get_color(node->piece_type) == WHITE ? 'W' : 'B',
                       position_get_file(node->to[0]), position_get_rank(node->to[1]), node->score);
        }
    }
    cchess_log("<END>\n");
}

void move_set_score(move_t *move, int score) {
    move->score = score;
}

int move_get_score(move_t *move) {
    return move->score;
}

char *move_get_to_position(move_t *move) {
    return move->to;
}

char *move_get_from_position(move_t *move) {
    return move->from;
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

    cchess_log("curr (%d) > bst (%d) ? (%d)\n", a_eval, b_eval, a_eval > b_eval);

    return a_eval > b_eval;
}

int __move_eval(move_t **path, int depth) {
    int result = 0;
    for (int i = 0; i < depth; i++) {
        result += (depth - i) * move_get_score(path[i]);
    }
    return result;
}

void move_cpy(move_t **src, move_t **dst, int depth) {
    for (int i = 0; i < depth; i++) {
        move_t *src_node = src[i];
        move_t *dst_node = dst[i];

        move_set_piece_type(dst_node, move_get_piece_type(src_node));
        move_set_score(dst_node, move_get_score(src_node));
        move_set_from_position(dst_node, move_get_from_position(src_node));
        move_set_to_position(dst_node, move_get_to_position(src_node));
    }
}
