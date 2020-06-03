#ifndef CCHESS_REQUEST_EVAL_H
#define CCHESS_REQUEST_EVAL_H

void client_request_eval(int client_sd, char *fen, char *from, char *to, int depth);

#endif //CCHESS_REQUEST_EVAL_H
