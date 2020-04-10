#ifndef CCHESS_PATH_NODE_H
#define CCHESS_PATH_NODE_H

#include "piece.h"

typedef struct path_node path_node_t;

path_node_t *path_node_new();

void path_node_ctor(path_node_t *);

void path_node_dtor(path_node_t *);

position_t *path_node_get_to_position(path_node_t *);

position_t *path_node_get_from_position(path_node_t *);

void path_node_set_score(path_node_t *, int);

void path_node_set_from_position(path_node_t *, position_t *);

void path_node_set_to_position(path_node_t *, position_t *);

int path_node_get_score(path_node_t *);

void path_node_set_piece_type(path_node_t *, piece_type_t);

piece_type_t path_node_get_piece_type(path_node_t *);

void path_node_print(path_node_t **, int);

int path_node_cmpr(path_node_t **, path_node_t **, int);

void path_node_cpy(path_node_t **src, path_node_t **dst, int);

#endif //CCHESS_PATH_NODE_H
