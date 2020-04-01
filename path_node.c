#include <stdlib.h>
#include <stdio.h>
#include "path_node.h"

struct path_node {
    piece_t *piece;
    position_t *position;
    int score;
};

path_node_t *path_node_new() {
    return (path_node_t *) malloc(sizeof(path_node_t));
}

void path_node_ctor(path_node_t *node, piece_t *piece, position_t *position) {
    node->piece = piece;
    node->position = position;
    node->score = -9999;
}

void path_node_dtor(path_node_t *node) {
}

void path_node_print(path_node_t **path, int size) {
    for (int i = 0; i < size; i++) {
        path_node_t *node = path[i];
        if (piece_get_type(node->piece) != NONE) {
            printf("(%c:%c%d=%d) -> ", piece_tpye_to_char(piece_get_type(node->piece)), position_get_x(node->position) + 'a', 8 - position_get_y(node->position), node->score);
        }
    }
    printf("X\n");
}

void path_node_set_score(path_node_t *node, int score) {
    node->score = score;
}

int path_node_get_score(path_node_t *node) {
    return node->score;
}

position_t *path_node_get_position(path_node_t *node) {
    return node->position;
}

piece_t *path_node_get_piece(path_node_t *node) {
    return node->piece;
}
