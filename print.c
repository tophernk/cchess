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

    switch (p % 10) {
        case PAWN_W:
        case PAWN_B:
            printf("P");
            break;
        case KNIGHT_W:
        case KNIGHT_B:
            printf("N");
            break;
        case BISHOP_W:
        case BISHOP_B:
            printf("B");
            break;
        case ROOK_W:
        case ROOK_B:
            printf("R");
            break;
        case QUEEN_W:
        case QUEEN_B:
            printf("Q");
            break;
        case KING_W:
        case KING_B:
            printf("K");
            break;
        default:
            printf(" ");
    }

    if (p > QUEEN_W) {
        printf("*  ");
    } else {
        printf("   ");
    }
}
