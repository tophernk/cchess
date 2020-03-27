#include "defintions.h"

int evalConf(config_t *config, piece_color_t color) {
    int white = 0;
    int black = 0;
    for (int i = 0; i < sizeof(config->white) / sizeof(config->white[0]); i++) {
        white += config->white[i].type;
        black += config->black[i].type;
    }
    return color == BLACK ? black - white : white - black;
}