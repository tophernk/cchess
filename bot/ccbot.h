#ifndef CCHESS_CCBOT_H
#define CCHESS_CCBOT_H

#define DEPTH 3
#define MOVE_EXECUTED(x) (x != -9999)

int ccbot_execute_move(config_t *);

#endif //CCHESS_CCBOT_H
