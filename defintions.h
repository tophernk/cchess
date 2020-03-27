#ifndef CCHESS_DEFINTIONS_H
#define CCHESS_DEFINTIONS_H

#include "pieces.h"

#define BOARD_SIZE 8
#define MAX_POSITIONS 27
#define NUMBER_OF_PIECES 16
#define DEPTH 3
#define FILE_OFFSET 'a'

typedef struct {
    int x;
    int y;
} pos_t;

typedef struct {
    piece_type_t type;
    pos_t current_position;
    pos_t available_positions[MAX_POSITIONS];
} piece_t;

typedef struct {
    piece_t *piece;
    pos_t to_position;
} move_t;

typedef struct {
    piece_type_t piece;
    pos_t position;
    int score;
    int piece_index;
} path_node_t;

typedef struct {
    piece_type_t board[BOARD_SIZE][BOARD_SIZE];
    piece_t white[NUMBER_OF_PIECES];
    piece_t black[NUMBER_OF_PIECES];
} config_t;

void printBoard(config_t *);

void printSolidLine();

void printIntermediateLine();

void printPiece(piece_type_t);

void print_eval_move(move_t move, int score);

void print_path(path_node_t *path, int size);

char piece_to_char(piece_type_t);

int move_piece(move_t move, config_t *);

int isValidMove(config_t *, int, int, int, int);

int abs(int);

int is_valid_pawn_move(int xmove, int ymove, int yfrom, int yto, piece_type_t piece, piece_type_t piece_at_to_position);

int is_valid_knight_move(pos_t from, pos_t to);

int is_valid_rook_move(pos_t from, pos_t to, config_t *cfg);

int is_valid_bishop_move(pos_t from, pos_t to, config_t *cfg);

int is_valid_queen_move(pos_t from, pos_t to, config_t *cfg);

int cpuMove(config_t *);

piece_t *getPiece(int, int, config_t *);

void update_available_positions(config_t *);

void determine_available_positions(piece_t *, config_t *);

config_t init();

piece_t createPiece(piece_type_t type, int x, int y, config_t *cfg);

void remove_piece(int x, int y, config_t *cfg);

move_t calculate_move(config_t *cfg);

int evalConf(config_t *config, piece_color_t color);

void invalidate_position(pos_t *position);

#endif //CCHESS_DEFINTIONS_H