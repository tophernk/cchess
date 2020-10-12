#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>

#include "move_service.h"
#include "../../eval/request/request_eval.h"
#include "../../stream_server.h"

typedef struct {
    char fen[100];
    char from_pos[2];
    char to_pos[2];
    int depth;
    bool white_to_move;
    move_t *best_move;
} move_eval_type_t;

int _connect(const char *host, const int port);

pthread_mutex_t mutex;

void *request_handler(void *arg) {
    int sd = *((int *) arg);
    free((int *) arg);

    config_t *config = config_new();
    char buffer[100];
    while (1) {
        int result = read(sd, buffer, 100);
        if (result == 0 || result == -1) {
            break;
        }
        config_fen_in(config, buffer);
        execute_best_move(config, 2);
        char fen_out[100];
        config_fen_out(config, fen_out);
        write(sd, fen_out, 100);
    }
    config_dtor(config);
    free(config);
    return NULL;
}

void *worker_eval_move(void *arg) {
    move_eval_type_t *args = (move_eval_type_t *) arg;

    int sd = -1;
    while (sd == -1) {
        sd = _connect("evalservice", 1025);
        if(sd == -1) {
            sleep(1);
        }
    }

    int eval = client_request_eval(sd, args->fen, args->depth);
    pthread_mutex_lock(&mutex);
    move_t *best_move = args->best_move;
    int best_eval = move_get_score(best_move);
    bool better = args->white_to_move ? eval > best_eval : eval < best_eval;
    if (better) {
        move_set_from_position(best_move, args->from_pos);
        move_set_to_position(best_move, args->to_pos);
        move_set_score(best_move, eval);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void execute_best_move(config_t *config, int depth) {
    pthread_t threads[100];
    int thread_i = 0;

    config_t *tmp_config = config_new();
    config_ctor(tmp_config);
    config_copy(config, tmp_config);

    piece_t **pieces = config_get_pieces_of_active_color(config);
    piece_t *current_piece;
    bool white_to_move = config_is_white_to_move(config);
    int best_eval = white_to_move ? -9999 : 9999;

    move_t *best_move = move_new();
    move_ctor(best_move);
    move_set_score(best_move, best_eval);

    move_t *current_move = move_new();
    move_ctor(current_move);

    move_eval_type_t args[100];

    pthread_mutex_init(&mutex, NULL);
    int *eval = (int *) malloc(sizeof(int));

    for (int i = 0; i < NUMBER_OF_PIECES; i++) {
        current_piece = pieces[i];
        char *from_pos = piece_get_current_position(current_piece);
        if (*from_pos == '-') {
            break;
        }
        for (int ii = 0; ii < MAX_POSITIONS; ii++) {
            char *to_pos = piece_get_available_position(current_piece, ii);
            if (*to_pos == '-') {
                break;
            }

            move_set_from_position(current_move, from_pos);
            move_set_to_position(current_move, to_pos);

            config_execute_move(tmp_config, current_move, eval);

            char fen[100];
            config_fen_out(tmp_config, fen);
            strcpy(args[thread_i].fen, fen);
            strncpy(args[thread_i].from_pos, from_pos, 2);
            strncpy(args[thread_i].to_pos, to_pos, 2);
            args[thread_i].depth = depth;
            args[thread_i].white_to_move = white_to_move;
            args[thread_i].best_move = best_move;

            int thread_not_created = pthread_create(&threads[thread_i], NULL, worker_eval_move, &args[thread_i]);
            thread_i++;
            if (thread_not_created) {
                fprintf(stderr, "could not start eval thread\n");
                exit(1);
            }
            config_copy(config, tmp_config);
        }
    }

    // wait for workers to finish
    for (int i = 0; i < thread_i; i++) {
        pthread_join(threads[i], NULL);
    }

    config_execute_move(config, best_move, eval);

    pthread_mutex_destroy(&mutex);
    free(eval);
    free(current_move);
    free(best_move);
}

int _connect(const char *host, const int port) {
    // create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        fprintf(stderr, "could not create client socket: %s\n", strerror(errno));
        exit(1);
    }

    // get host address
    struct hostent *host_entry;
    host_entry = gethostbyname(host);
    if (!host_entry) {
        fprintf(stderr, "could not resolve host name %s\n", host);
        return -1;
    }
    // prep socket

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = *((struct in_addr *) host_entry->h_addr);
    addr.sin_port = htons(port);

    // connect
    int result = connect(sd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        close(sd);
        fprintf(stderr, "could not connect to server %s\n", strerror(errno));
        return -1;
    }
    return sd;
}

void accept_forever(int server_sd) {
    while (1) {
        int client_sd = accept(server_sd, NULL, NULL);
        if (client_sd == -1) {
            close(server_sd);
            fprintf(stderr, "could not accept client: %s\n", strerror(errno));
            exit(1);
        }
        int *arg = (int *) malloc(sizeof(int));
        *arg = client_sd;
        pthread_t request_handler_thread;
        int result = pthread_create(&request_handler_thread, NULL, request_handler, arg);
        if (result) {
            close(client_sd);
            close(server_sd);
            free(arg);
            fprintf(stderr, "could not start client thread\n");
            exit(1);
        }
    }
}