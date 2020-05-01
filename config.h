#ifndef CCHESS_CONFIG_H
#define CCHESS_CONFIG_H

#include "piece.h"
#include "move.h"

#define MOVE_EXECUTED(x) (x != -9999)

typedef struct config config_t;

config_t *config_new();

void config_ctor(config_t *);

void config_dtor(config_t *);

void config_fen_in(config_t *, char *);

char *config_fen_out(config_t *);

void config_copy(config_t *src, config_t *dst);

int config_eval(config_t *config, piece_color_t color);

void config_update_available_positions(config_t *);

piece_t *config_get_piece(config_t *, piece_color_t, position_t *);

void config_add_piece(config_t *, piece_type_t, int x, int y, piece_color_t, int index);

void config_remove_piece(config_t *cfg, position_t *);

int config_execute_move(config_t *, move_t *);

int config_move_cpu(config_t *);

void config_calculate_move(config_t *, move_t *);

int config_en_passant(config_t *, int x, int y);

/**
 *
 * @return
 * 0 - invalid moves
 * 1 - valid regular moves
 * 2 - valid en passants
 * 3 to 6 - valid castles (black/white short/long)
 */
int config_valid_move(config_t *, piece_t *, int xto, int yto);

void config_print(config_t *);

int config_move_available(config_t *, piece_color_t);

void config_enable_short_castles(config_t *, piece_color_t);

void config_disable_short_castles(config_t *, piece_color_t);

void config_enable_long_castles(config_t *, piece_color_t);

void config_disable_long_castles(config_t *, piece_color_t);

#endif //CCHESS_CONFIG_H
