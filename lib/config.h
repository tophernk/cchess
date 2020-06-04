#ifndef CCHESS_CONFIG_H
#define CCHESS_CONFIG_H

#include <stdbool.h>
#include "piece.h"
#include "move.h"

#define BOARD_SIZE 8
#define NUMBER_OF_PIECES 16

typedef struct config config_t;

config_t *config_new();

void config_ctor(config_t *);

void config_dtor(config_t *);

void config_fen_in(config_t *, char *);

void config_fen_out(config_t *, char *);

void config_copy(config_t *src, config_t *dst);

int config_eval(config_t *config);

int config_eval_to_depth(config_t *config, int depth);

void config_update_available_positions(config_t *);

piece_t *config_get_piece(config_t *, piece_color_t, char position[2]);

piece_t **config_get_pieces_of_active_color(config_t *);

void config_add_piece(config_t *, piece_type_t, int x, int y, piece_color_t, int index);

void config_remove_piece(config_t *cfg, char *);

int config_execute_move(config_t *, move_t *, int *eval_result);

int config_en_passant(config_t *, char *);

void config_print(config_t *);

int config_move_available(config_t *, piece_color_t);

void config_enable_short_castles(config_t *, piece_color_t);

void config_disable_short_castles(config_t *, piece_color_t);

void config_enable_long_castles(config_t *, piece_color_t);

void config_disable_long_castles(config_t *, piece_color_t);

bool config_is_white_to_move(config_t *);

void config_set_white_to_move(config_t *, bool);

void config_determine_best_move(config_t *, int depth, move_t*);

int config_play_best_move(config_t *, int depth);

#endif //CCHESS_CONFIG_H
