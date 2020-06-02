#include <stdlib.h>

#include "logger.h"
#include "config.h"
#include "position.h"
#include "ccbot.h"

int ccbot_execute_move(config_t *conf) {
    cchess_log("cpu move...\n");
    int result = 0;
    move_t *next_move = move_new();
    move_ctor(next_move);
    config_determine_best_move(conf, 3, next_move);

    if (move_get_piece_type(next_move) > NONE && position_valid(move_get_to_position(next_move))) {
        result = config_execute_move(conf, next_move);
    }

    free(next_move);

    return MOVE_EXECUTED(result);
}
