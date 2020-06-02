#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ccbot.h"
#include "logger.h"

int main(int argc, char *argv[]) {
    remove(CCHESS_LOG);

    config_t *config = config_new();
    if (argc > 1) {
        config_fen_in(config, argv[1]);
    } else {
        config_ctor(config);
        config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    }

    config_print(config);

    move_t *move = move_new();
    move_ctor(move);
    char from[2];
    char to[2];
    int pieceMoved = 1;

    int *score = (int *) malloc(sizeof(int));

    while (pieceMoved) {
        while (scanf("%c %c %c %c", &from[0], &from[1], &to[0], &to[1]) != 4) {
            while ((from[0] = getchar()) != EOF && from[0] != '\n');
            printf("invalid input\n");
        }

        piece_t *piece = config_get_piece(config, WHITE, from);
        if (piece == NULL) {
            break;
        }

        move_set_from_position(move, from);
        move_set_to_position(move, to);
        move_set_piece_type(move, piece_get_type(piece));

        pieceMoved = config_execute_move(config, move, score);
        if (pieceMoved) {
            config_print(config);
        } else {
            printf("invalid move\n");
        }

        getchar(); // discard newline from input
        if (pieceMoved) {
            char config_before_move[100];
            char config_after_move[100];

            memset(config_before_move, 0, 100);
            memset(config_after_move, 0, 100);

            config_fen_out(config, config_before_move);
            ccbot_execute_move(config);
            config_fen_out(config, config_after_move);

            pieceMoved = strncmp(config_before_move, config_after_move, 100);
            config_print(config);
            if (!config_move_available(config, WHITE)) {
                printf("no move available for white\n");
                break;
            }
        }
    }
    printf("exit.. (no piece moved)\n");

    free(score);
    free(move);
    config_dtor(config);
    free(config);

    return 0;
}
