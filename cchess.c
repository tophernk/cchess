#include <stdio.h>

#define BOARD_SIZE 8

#define PAWN_W 1
#define KNIGHT_W 2
#define BISHOP_W 3
#define ROOK_W 5
#define QUEEN_W 9
#define KING_W 6

#define PAWN_B 11
#define KNIGHT_B 12
#define BISHOP_B 13
#define ROOK_B 15
#define QUEEN_B 19
#define KING_B 16

#define FILE_OFFSET 'a'

void printBoard(int[BOARD_SIZE][BOARD_SIZE]);
void printSolidLine();
void printIntermediateLine();
void printPiece(int);
int movePiece(int, int, int, int, int[BOARD_SIZE][BOARD_SIZE]);
int isBoardPosition(int, int);

int main() {
    static int board[BOARD_SIZE][BOARD_SIZE] = {
        { ROOK_B, KNIGHT_B, BISHOP_B, QUEEN_B, KING_B, BISHOP_B, KNIGHT_B, ROOK_B },
        { PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W },
        { ROOK_W, KNIGHT_W, BISHOP_W, QUEEN_W, KING_W, BISHOP_W, KNIGHT_W, ROOK_W }
    };

    printBoard(board);

    char pos1_x, pos2_x;
    int pos1_y, pos2_y;

    scanf("%c %d %c %d", &pos1_x, &pos1_y, &pos2_x, &pos2_y);

    pos1_x -= FILE_OFFSET;
    pos2_x -= FILE_OFFSET;

    pos1_y = (pos1_y - BOARD_SIZE) * -1;
    pos2_y = (pos2_y - BOARD_SIZE) * -1;

    printf("%d %d %d %d\n", pos1_y, pos1_x, pos2_y, pos2_x);
    movePiece(pos1_y, pos1_x, pos2_y, pos2_x, board);
    printBoard(board);

    return 0;
}

int movePiece(int x1, int y1, int x2, int y2, int board[BOARD_SIZE][BOARD_SIZE]) {
    if(isBoardPosition(x1, y2) && isBoardPosition(x2, y2)) {
        board[x2][y2] = board[x1][y1];
        board[x1][y1] = 0;
        return 0;
    }
    return -1;
}

int isBoardPosition(int x, int y) {
    if(x < 0 || x > BOARD_SIZE) {
        return 0;
    }
    if(y < 0 || y > BOARD_SIZE) {
        return 0;
    }
    return 1;
}

void printBoard(int board[8][8]) {
    for(int x = 0; x < BOARD_SIZE; x++) {
        printSolidLine();
        printIntermediateLine();
        for(int y = 0; y < BOARD_SIZE; y++) {
            printPiece(board[x][y]);
        }
        printf("\n");
        printIntermediateLine();
    }
    printSolidLine();
}

void printSolidLine() {
    for(int i = 0; i < BOARD_SIZE * 8; i++) {
            printf("#");
        }
        printf("\n");
    }

void printIntermediateLine() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        printf("#       ");
    }
    printf("\n");
}

void printPiece(int p) {
    printf("#   ");

    switch(p % 10) {
        case 1: printf("P"); break;
        case 2: printf("N"); break;
        case 3: printf("B"); break;
        case 5: printf("R"); break;
        case 9: printf("Q"); break;
        case 6: printf("K"); break;
        default: printf(" ");
    };

    if(p > 10) {
        printf("*  ");
    }
    else {
        printf("   ");
    }
}