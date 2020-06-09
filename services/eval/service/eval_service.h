#ifndef CCHESS_EVAL_SERVICE_H
#define CCHESS_EVAL_SERVICE_H

#include "config.h"

int eval_board(config_t *config, int depth);
void eval_req_to_args(char *request, config_t *config, int *depth);
void eval_result_to_rsp(char *response, int);

#endif //CCHESS_EVAL_SERVICE_H
