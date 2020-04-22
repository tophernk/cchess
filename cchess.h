#ifndef CCHESS_CCHESS_H
#define CCHESS_CCHESS_H

#define BOARD_SIZE 8
#define NUMBER_OF_PIECES 16
#define DEPTH 2
#define FILE_OFFSET 'a'

#include "config.h"

void cchess_init(config_t *config);

#endif //CCHESS_CCHESS_H