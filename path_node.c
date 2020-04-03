#include <stdlib.h>
#include <stdio.h>
#include "path_node.h"

struct path_node {
    piece_type_t piece_type;
    position_t *from_position;
    position_t *to_position;
    int score;
};

path_node_t *path_node_new() {
    return (path_node_t *) malloc(sizeof(path_node_t));
}

void path_node_ctor(path_node_t *node) {
    node->piece_type = NONE;
    node->from_position = position_new();
    position_ctor(node->from_position);
    node->to_position = position_new();
    position_ctor(node->to_position);
    node->score = -9999;
}

void path_node_dtor(path_node_t *node) {
    position_dtor(node->from_position);
    free(node->from_position);
    position_dtor(node->to_position);
    free(node->to_position);
}

void path_node_set_to_position(path_node_t *node, position_t *position) {
    position_copy(position, node->to_position);
}

void path_node_set_from_position(path_node_t *node, position_t *position) {
    position_copy(position, node->from_position);
}

void path_node_print(path_node_t **path, int size) {
    for (int i = 0; i < size; i++) {
        path_node_t *node = path[i];
        if (node->piece_type != NONE) {
            printf("(%c:%c%d=%d) -> ", piece_tpye_to_char(node->piece_type), position_get_x(node->to_position) + 'a', 8 - position_get_y(node->to_position),
                   node->score);
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

position_t *path_node_get_to_position(path_node_t *node) {
    return node->to_position;
}

position_t *path_node_get_from_position(path_node_t *node) {
    return node->from_position;
}

void path_node_set_piece_type(path_node_t *node, piece_type_t type) {
    node->piece_type = type;
}
