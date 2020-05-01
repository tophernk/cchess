#include <stdio.h>
#include <stdlib.h>
#include "cchess.h"
#include "config.h"
#include "logger.h"

int main(int argc, char *argv[]) {
    srand(0);
    remove(CCHESS_LOG);

    config_t *config = config_new();
    if (argc > 1) {
        config_fen_in(config, argv[1]);
    } else {
        config_ctor(config);
        config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    }
    config_print(config);

    position_t *from = position_new();
    position_ctor(from);
    position_t *to = position_new();
    position_ctor(to);

    move_t *move = move_new();
    move_ctor(move);

    int pieceMoved = 1;
    while (pieceMoved) {
        char from_x, to_x;
        int from_y, to_y;

        while (scanf("%c %d %c %d", &from_x, &from_y, &to_x, &to_y) != 4) {
            while ((from_x = getchar()) != EOF && from_x != '\n');
            printf("invalid input\n");
        }

        position_set_x(from, from_x - FILE_OFFSET);
        position_set_y(from, (from_y - BOARD_SIZE) * -1);
        position_set_x(to, to_x - FILE_OFFSET);
        position_set_y(to, (to_y - BOARD_SIZE) * -1);

        piece_t *piece = config_get_piece(config, WHITE, from);

        if (piece == NULL) {
            break;
        }

        move_set_from_position(move, from);
        move_set_to_position(move, to);
        move_set_piece_type(move, piece_get_type(piece));

        int eval = config_execute_move(config, move);
        if (MOVE_EXECUTED(eval)) {
            pieceMoved = 1;
            config_print(config);
        } else {
            printf("invalid move\n");
            pieceMoved = 0;
        }

        getchar(); // discard newline from input
        if (pieceMoved) {
            pieceMoved = config_move_cpu(config);
            if (!config_move_available(config, WHITE)) {
                printf("no move available for white\n");
                break;
            }
        }
    }
    printf("exit.. (no piece moved)\n");

    position_dtor(from);
    free(from);
    position_dtor(to);
    free(to);
    move_dtor(move);
    free(move);
    config_dtor(config);
    free(config);

    return 0;
}
