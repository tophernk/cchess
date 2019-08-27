#include <stdio.h>

#define BOARD_SIZE 8

#define PAWN_W 1
#define KNIGHT_W 2
#define BISHOP_W 3
#define ROUGE_W 5
#define QUEEN_W 9
#define KING_W 10

#define PAWN_B 11
#define KNIGHT_B 12
#define BISHOP_B 13
#define ROUGE_B 15
#define QUEEN_B 19
#define KING_B 20

#define a 0
#define b 1
#define c 2
#define d 3
#define e 4
#define f 5
#define g 6 
#define h 7

#define FILE_OFFSET 'a'
#define ROW_OFFSET 1

static int characterMap[8] = {
    a, b, c, d, e, f, g, h
};

void printBoard(int[BOARD_SIZE][BOARD_SIZE]);

int main() {
    int board[BOARD_SIZE][BOARD_SIZE] = {
        { ROUGE_B, KNIGHT_B, BISHOP_B, QUEEN_B, KING_B, BISHOP_B, KNIGHT_B, ROUGE_B },
        { PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W },
        { ROUGE_W, KNIGHT_W, BISHOP_W, QUEEN_W, KING_W, BISHOP_W, KNIGHT_W, ROUGE_W }
    };

    printBoard(board);

    char pos1_x, pos2_x;
    int pos1_y, pos2_y;

    scanf("%c %d %c %d", &pos1_x, &pos1_y, &pos2_x, &pos2_y);

    pos1_x -= FILE_OFFSET;
    pos2_x -= FILE_OFFSET;

    pos1_y -= ROW_OFFSET;
    pos2_y -= ROW_OFFSET;

    printf("%d %d %d %d\n", pos1_x, pos1_y, pos2_x, pos2_y);

    return 0;
}

void printBoard(int board_copy[8][8]) {
    for(int x = 0; x < BOARD_SIZE; x++) {
        for(int y = 0; y < BOARD_SIZE; y++) {
            printf("%d", board_copy[x][y]);
        }
        printf("\n");
    }
}