#ifndef CCHESS_CONFIG_H
#define CCHESS_CONFIG_H

#include "piece.h"
#include "path_node.h"

typedef struct config config_t;

config_t *config_new();

void config_ctor(config_t *);

void config_dtor(config_t *);

void config_copy(config_t *src, config_t *dst);

int config_eval(config_t *config, piece_color_t color);

void config_update_available_positions(config_t *);

piece_t *config_get_piece(config_t *, piece_color_t, position_t *);

void config_remove_piece(config_t *cfg, position_t *);

int config_execute_move(config_t *, path_node_t *);

int config_move_cpu(config_t *);

void config_calculate_move(config_t *, path_node_t *);

int config_valid_move(config_t *, piece_t *, int xto, int yto);

void config_print(config_t *);

int config_move_available(config_t *, piece_color_t);

#endif //CCHESS_CONFIG_H
