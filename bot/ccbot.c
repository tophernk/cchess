#include "logger.h"
#include "config.h"
#include "ccbot.h"

void ccbot_execute_move(config_t *conf) {
    cchess_log("bot move...\n");
    config_play_best_move(conf, 3);
}
