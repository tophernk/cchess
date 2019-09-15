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
int movePiece(int *, int *);
int *getBoardPosition(int, int, int[BOARD_SIZE][BOARD_SIZE]);
int isValidMove(int, int, int, int, int[BOARD_SIZE][BOARD_SIZE]);
int abs(int);

int main()
{
    static int board[BOARD_SIZE][BOARD_SIZE] = {
        {ROOK_B, KNIGHT_B, BISHOP_B, QUEEN_B, KING_B, BISHOP_B, KNIGHT_B, ROOK_B},
        {PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B, PAWN_B},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W, PAWN_W},
        {ROOK_W, KNIGHT_W, BISHOP_W, QUEEN_W, KING_W, BISHOP_W, KNIGHT_W, ROOK_W}};

    printBoard(board);

    int pieceMoved = 1;
    while (pieceMoved)
    {
        char from_file, to_file;
        int from_rank, to_rank;

        while (scanf("%c %d %c %d", &from_file, &from_rank, &to_file, &to_rank) != 4)
        {
            while ((from_file = getchar()) != EOF && from_file != '\n')
                ;
            printf("invalid input\n");
        }

        from_file -= FILE_OFFSET;
        to_file -= FILE_OFFSET;

        from_rank = (from_rank - BOARD_SIZE) * -1;
        to_rank = (to_rank - BOARD_SIZE) * -1;

        int *from_pos = getBoardPosition(from_file, from_rank, board);
        int *to_pos = getBoardPosition(to_file, to_rank, board);

        if (from_pos == NULL || to_pos == NULL)
        {
            break;
        }

        if (isValidMove(from_file, from_rank, to_file, to_rank, board))
        {
            pieceMoved = movePiece(from_pos, to_pos);
            printBoard(board);
        }
        else
        {
            printf("invalid move\n");
            pieceMoved = 0;
        }

        getchar(); // discard newline from input
    }

    printf("exit.. (no piece moved)\n");

    return 0;
}

int isValidMove(int xfrom, int yfrom, int xto, int yto, int board[BOARD_SIZE][BOARD_SIZE])
{
    int result = 1;
    int piece = board[yfrom][xfrom];
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    if (piece % 10 == 1)
    {
        if (xmove > 1)
        {
            result = 0;
        }
        if (xmove == 1 && board[yto][xto] == 0)
        {
            result = 0;
        }
        if (piece == PAWN_W && yfrom - yto != 1)
        {
            result = 0;
        }
        if (piece == PAWN_B && yfrom - yto != -1)
        {
            result = 0;
        }
    }
    else if (piece % 10 == 2)
    {
        int xmove = abs(xfrom - xto);
        int ymove = abs(yfrom - yto);

        if (xmove == 1 && ymove == 2)
            return 1;
        if (xmove == 2 && ymove == 1)
            return 1;
        return 0;
    }
    else if (piece % 10 == 3)
    {
        if (xmove != ymove)
            return 0;
    }
    else
    {
        result = 0;
        printf("can only move pawns so far\n");
    }
    return result;
}

int abs(int x)
{
    if (x < 0)
        return x * -1;
    return x;
}

int movePiece(int *from, int *to)
{
    if (*from != 0)
    {
        *to = *from;
        *from = 0;
        return 1;
    }
    return 0;
}

int *getBoardPosition(int rank, int file, int board[BOARD_SIZE][BOARD_SIZE])
{
    if (rank < 0 || rank > BOARD_SIZE)
    {
        return NULL;
    }
    if (file < 0 || file > BOARD_SIZE)
    {
        return NULL;
    }
    return &board[file][rank];
}

void printBoard(int board[8][8])
{
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        printSolidLine();
        printIntermediateLine();
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            printPiece(board[x][y]);
        }
        printf("\n");
        printIntermediateLine();
    }
    printSolidLine();
}

void printSolidLine()
{
    for (int i = 0; i < BOARD_SIZE * 8; i++)
    {
        printf("#");
    }
    printf("\n");
}

void printIntermediateLine()
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        printf("#       ");
    }
    printf("\n");
}

void printPiece(int p)
{
    printf("#   ");

    switch (p % 10)
    {
    case 1:
        printf("P");
        break;
    case 2:
        printf("N");
        break;
    case 3:
        printf("B");
        break;
    case 5:
        printf("R");
        break;
    case 9:
        printf("Q");
        break;
    case 6:
        printf("K");
        break;
    default:
        printf(" ");
    };

    if (p > 10)
    {
        printf("*  ");
    }
    else
    {
        printf("   ");
    }
}