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

    for (int i = 0; i < MAX_POSITIONS; i++) {
        invalidate_position(&tmp.available_positions[i]);
    }
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
    for (int i = 0; i < BOARD_SIZE * 2; i++) {
        conf.white[i] = createPiece(NONE, -1, -1, &conf);
        conf.black[i] = createPiece(NONE, -1, -1, &conf);
    }

//    conf.white[0] = createPiece(PAWN_W, 0, 6, &conf);
    conf.white[1] = createPiece(PAWN_W, 1, 6, &conf);
    conf.white[2] = createPiece(PAWN_W, 3, 6, &conf);
    conf.white[3] = createPiece(KNIGHT_W, 1, 7, &conf);
    conf.white[4] = createPiece(ROOK_W, 0, 7, &conf);
    conf.white[5] = createPiece(BISHOP_W, 2, 7, &conf);
    conf.white[6] = createPiece(QUEEN_W, 3, 7, &conf);

    conf.black[0] = createPiece(PAWN_B, 0, 1, &conf);
    conf.black[1] = createPiece(KNIGHT_B, 1, 0, &conf);

    update_available_positions(&conf);

    return conf;
}

int main() {
    config_t conf = init();
    printBoard(&conf);

    int pieceMoved = 1;
    while (pieceMoved) {
        char from_x, to_x;
        int from_y, to_y;

        while (scanf("%c %d %c %d", &from_x, &from_y, &to_x, &to_y) != 4) {
            while ((from_x = getchar()) != EOF && from_x != '\n');
            printf("invalid input\n");
        }

        from_x -= FILE_OFFSET;
        to_x -= FILE_OFFSET;

        from_y = (from_y - BOARD_SIZE) * -1;
        to_y = (to_y - BOARD_SIZE) * -1;

        piece_t *piece = getPiece(from_x, from_y, &conf);

        if (piece == NULL) {
            break;
        }

        pos_t to;
        to.x = to_x;
        to.y = to_y;

        move_t move;
        move.to_position = to;
        move.piece = piece;

        int eval = move_piece(move, &conf);
        if ((eval != -9999)) {
            pieceMoved = 1;
            printBoard(&conf);
        } else {
            printf("invalid move\n");
            pieceMoved = 0;
        }

        getchar(); // discard newline from input
        if (pieceMoved) {
            pieceMoved = cpuMove(&conf);

            size_t size = sizeof(conf.white) / sizeof(conf.white[0]);
            for (int i = 0; i < size; i++) {
                if (conf.white[i].type != NONE)
                    break;
                if (i == size - 1)
                    goto exit;
            }
        }
    }
    exit:
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
}

void invalidate_position(pos_t *position) {
    position->x = -1;
    position->y = -1;
}

void determine_available_positions(piece_t *p, config_t *conf) {
    int valid_pos_counter = 0;
    int piece_x = p->current_position.x;
    int piece_y = p->current_position.y;
    for (int to_x = 0; to_x < BOARD_SIZE; to_x++) {
        for (int to_y = 0; to_y < BOARD_SIZE; to_y++) {
            if (isValidMove(conf, piece_x, piece_y, to_x, to_y)) {
                pos_t validPos;
                validPos.x = to_x;
                validPos.y = to_y;
                p->available_positions[valid_pos_counter] = validPos;
                valid_pos_counter++;
            }
        }
    }
    // invalidate remaining positions
    for (int i = valid_pos_counter; i < sizeof(p->available_positions) / sizeof(p->available_positions[0]); i++) {
        invalidate_position(&p->available_positions[i]);
    }
}

piece_t *getPiece(int x, int y, config_t *conf) {
    for (int i = 0; i < sizeof(conf->white) / sizeof(conf->white[0]); i++) {
        if (conf->white[i].current_position.x == x && conf->white[i].current_position.y == y) {
            return &conf->white[i];
        }
    }
    return NULL;
}