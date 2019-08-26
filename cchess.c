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

    for(int x = 0; x < BOARD_SIZE; x++) {
        for(int y = 0; y < BOARD_SIZE; y++) {
            printf("%d", board[x][y]);
        }
        printf("\n");
    }
    return 0;
}