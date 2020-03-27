#include <stdio.h>
#include "defintions.h"

void printBoard(config_t *conf) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        printSolidLine();
        printIntermediateLine();
        printf("  %d ", 8 - x);
        for (int y = 0; y < BOARD_SIZE; y++) {
            printPiece(conf->board[y][x]);
        }
        printf("|\n");
        printIntermediateLine();
    }
    printSolidLine();
    printf("        A       B       C       D       E       F       G       H    \n");
}

void printSolidLine() {
    printf("    ");
    for (int i = 0; i < BOARD_SIZE * 8; i++) {
        printf("-");
    }
    printf("-\n");
}

void printIntermediateLine() {
    printf("    ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("|       ");
    }
    printf("|\n");
}

void printPiece(piece_type_t p) {
    printf("|   ");
    printf("%c", piece_to_char(p));

    if (p > QUEEN_W) {
        printf("*  ");
    } else {
        printf("   ");
    }
}

char piece_to_char(piece_type_t type) {
    switch (type) {
        case PAWN_W:
        case PAWN_B:
            return 'P';
        case KNIGHT_W:
        case KNIGHT_B:
            return 'N';
        case BISHOP_W:
        case BISHOP_B:
            return 'B';
        case ROOK_W:
        case ROOK_B:
            return 'R';
        case QUEEN_W:
        case QUEEN_B:
            return 'Q';
        case KING_W:
        case KING_B:
            return 'K';
        default:
            return ' ';
    }
}

void print_eval_move(move_t move, int score) {
    printf("%c:%c%d=%d ", piece_to_char(move.piece->type), move.to_position.x + 'a', 8 - move.to_position.y, score);
}

void print_path(path_node_t *path, int size) {
    for (int i = 0; i < size; i++) {
        path_node_t node = path[i];
        if (node.piece != NONE) {
            printf("(%c:%c%d=%d) -> ", piece_to_char(node.piece), node.position.x + 'a', 8 - node.position.y, node.score);
        }
    }
    printf("X\n");
}
