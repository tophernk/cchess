#include <stdio.h>
#include <stdlib.h>
#include "defintions.h"
#include "pieces.h"

piece_t createPiece(piece_type_t type, int x, int y, config_t *config) {
    piece_t tmp;
    tmp.type = type;

    pos_t p;
    p.x = x;
    p.y = y;
    tmp.current_position = p;

    config->board[tmp.current_position.x][tmp.current_position.y] = tmp.type;

    return tmp;
}

config_t init() {
    srand(0);

    config_t conf;

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            conf.board[x][y] = NONE;
        }
    }

    conf.white[0] = createPiece(PAWN_W, 6, 0, &conf);
    conf.white[1] = createPiece(KNIGHT_W, 7, 1, &conf);

    conf.black[0] = createPiece(PAWN_B, 1, 0, &conf);
    conf.black[1] = createPiece(KNIGHT_B, 0, 1, &conf);

    return conf;
}

int main() {
    config_t conf = init();
    printBoard(&conf);

    int pieceMoved = 1;
    while (pieceMoved) {
        char from_file, to_file;
        int from_rank, to_rank;

        while (scanf("%c %d %c %d", &from_file, &from_rank, &to_file, &to_rank) != 4) {
            while ((from_file = getchar()) != EOF && from_file != '\n');
            printf("invalid input\n");
        }

        from_file -= FILE_OFFSET;
        to_file -= FILE_OFFSET;

        from_rank = (from_rank - BOARD_SIZE) * -1;
        to_rank = (to_rank - BOARD_SIZE) * -1;

        struct piece *pc = getPiece(from_file, from_rank, &conf);
        int *to_pos = getBoardPosition(to_file, to_rank, &conf);

        if (pc == NULL || to_pos == NULL) {
            break;
        }

        if ((pieceMoved = movePiece(pc, to_pos, &conf))) {
            printBoard(&conf);
        } else {
            printf("invalid move\n");
            pieceMoved = 0;
        }

        getchar(); // discard newline from input
        if (pieceMoved == 1) {
            pieceMoved = cpuMove(&conf);
        }
    }

    printf("exit.. (no piece moved)\n");

    return 0;
}

int abs(int x) {
    if (x < 0)
        return x * -1;
    return x;
}

void update_available_positions(config_t *conf) {
    for (int i = 0; i < sizeof(conf->white) / sizeof(conf->white[0]); i++) {
        if (conf->white[i].type != NONE)
            determine_available_positions(&conf->white[i], conf);
    }
    for (int i = 0; i < sizeof(conf->black) / sizeof(conf->black[0]); i++) {
        if (conf->black[i].type != NONE)
            determine_available_positions(&conf->black[i], conf);
    }
    printf("all pieces updated\n");
}

void determine_available_positions(piece_t *p, config_t *conf) {
    int valid_pos_counter = 0;
    int piece_file = 0;
    int piece_rank = 0;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (x == p->current_position.x && y == p->current_position.y) {
                piece_file = x;
                piece_rank = y;
                goto found;
            }
        }
    }
    found:
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (isValidMove(conf, piece_rank, piece_file, y, x)) {
                pos_t validPos;
                validPos.x = x;
                validPos.y = y;
                p->available_positions[valid_pos_counter] = validPos;
                valid_pos_counter++;
            }
        }
    }
    // set remaining positions of piece to 0
    for (int i = valid_pos_counter; i < sizeof(p->available_positions) / sizeof(p->available_positions[0]); i++) {
        pos_t invalid_pos;
        invalid_pos.x = -1;
        invalid_pos.y = -1;
        p->available_positions[i] = invalid_pos;
    }
}

int *getBoardPosition(int rank, int file, config_t *conf) {
    if (rank < 0 || rank > BOARD_SIZE) {
        return NULL;
    }
    if (file < 0 || file > BOARD_SIZE) {
        return NULL;
    }
    return &conf->board[file][rank];
}

piece_t *getPiece(int rank, int file, config_t *conf) {
    for (int i = 0; i < sizeof(conf->white) / sizeof(conf->white[0]); i++) {
        if (conf->white[i].current_position.x == file && conf->white[i].current_position.y == rank) {
            return &conf->white[i];
        }
    }
    return NULL;
}