#include "logger.h"
#include "config.h"
#include "ccbot.h"

int ccbot_execute_move(config_t *conf) {
    cchess_log("bot move...\n");
    int result = config_play_best_move(conf, 3);
    return result;
}
