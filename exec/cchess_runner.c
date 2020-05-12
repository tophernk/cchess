#include <stdio.h>
#include <stdlib.h>
#include "config.h"
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

    config_move_cpu(config);

    config_dtor(config);
    free(config);

    return 0;
}