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

    position_t *fromp = position_new();
    position_ctor(fromp);
    position_t *top = position_new();
    position_ctor(top);

    move_t *move = move_new();
    move_ctor(move);
    char from[2];
    char to[2];
    int pieceMoved = 1;

    while (pieceMoved) {
        while (scanf("%c %c %c %c", &from[0], &from[1], &to[0], &to[1]) != 4) {
            while ((from[0] = getchar()) != EOF && from[0] != '\n');
            printf("invalid input\n");
        }

        position_set_x(fromp, position_get_x_(from[0]));
        position_set_y(fromp, position_get_y_(from[1]));
        position_set_x(top, position_get_x_(to[0]));
        position_set_y(top, position_get_y_(to[1]));

        piece_t *piece = config_get_piece(config, WHITE, from);
        if (piece == NULL) {
            break;
        }

        move_set_from_position(move, fromp);
        move_set_to_position(move, top);
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

    position_dtor(fromp);
    free(fromp);
    position_dtor(top);
    free(top);
    move_dtor(move);
    free(move);
    config_dtor(config);
    free(config);

    return 0;
}
