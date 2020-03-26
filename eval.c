#include "defintions.h"

int evalConf(config_t *in_conf) {
    int white = 0;
    int black = 0;
    for (int i = 0; i < sizeof(in_conf->white) / sizeof(in_conf->white[0]); i++) {
        white += in_conf->white[i].type;
        black += in_conf->black[i].type;
    }
    return black - white;
}