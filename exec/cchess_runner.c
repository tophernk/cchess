#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "logger.h"

int main(int argc, char *argv[]) {
    remove(CCHESS_LOG);

    config_t *config = config_new();
    if (argc > 1) {
        config_fen_in(config, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    } else {
        char fen_in[100];
        read(STDIN_FILENO, fen_in, 100);
        config_fen_in(config, fen_in);
    }

    config_move_cpu(config);
    char fen_out[100];
    config_fen_out(config, fen_out);
    printf("%s\n", fen_out);

    config_dtor(config);
    free(config);

    return 0;
}