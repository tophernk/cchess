#include <stdio.h>
#include <stdlib.h>

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

struct piece
{
    int type;
    int *current_position;
    int *available_positions[27];
};

struct move
{
    struct piece *p;
    int *to_position;
};

struct config
{
    int board[BOARD_SIZE][BOARD_SIZE];
    struct piece white[16];
    struct piece black[16];
};

void printBoard(struct config *);
void printSolidLine();
void printIntermediateLine();
void printPiece(int);
int movePiece(struct piece *, int *, struct config *);
int *getBoardPosition(int, int, struct config *);
int isValidMove(struct config *, int, int, int, int);
int abs(int);
int isValidRookMove(int, int);
int isValidBishopMove(int, int);
int isValidQueenMove(int, int);
int cpuMove(struct config *);
struct piece *getPiece(int, int, struct config *);
void update_available_positions(struct config *);
void determine_available_positions(struct piece *, struct config *);
struct config init();
struct piece createPiece(int, int *);
struct move calculateMove(struct config *, int);
int evalConf(struct config *);

struct move calculateMove(struct config *conf, int depth)
{
    struct move result;
    int i = rand() % 2;
    result.p = &conf->black[i];
    result.to_position = conf->black[i].available_positions[0];
    return result;
}

int evalConf(struct config *in_conf)
{
    int white;
    int black;
    for (int i = 0; i < sizeof(in_conf->white) / sizeof(in_conf->white[0]); i++)
    {
        white += in_conf->white[i].type;
        black += in_conf->black[i].type;
    }
    return white - black;
}

struct piece createPiece(int type, int *pos)
{
    struct piece tmp;
    tmp.type = type;
    tmp.current_position = pos;

    return tmp;
}

struct config init()
{
    srand(0);

    struct config conf = {};

    conf.white[0] = createPiece(PAWN_W, &conf.board[6][0]);
    conf.white[1] = createPiece(KNIGHT_W, &conf.board[7][1]);

    conf.black[0] = createPiece(PAWN_B, &conf.board[1][0]);
    conf.black[1] = createPiece(KNIGHT_B, &conf.board[0][1]);

    for (int i = 0; i < sizeof(conf.white) / sizeof(conf.white[0]); i++)
    {
        struct piece tmp = conf.white[i];
        if (tmp.type != 0)
        {
            *tmp.current_position = tmp.type;
        }
    }

    for (int i = 0; i < sizeof(conf.black) / sizeof(conf.black[0]); i++)
    {
        struct piece tmp = conf.black[i];
        if (tmp.type != 0)
        {
            *tmp.current_position = tmp.type;
        }
    }
    update_available_positions(&conf);

    return conf;
}

int main()
{
    struct config conf = init();
    printBoard(&conf);

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

        struct piece *pc = getPiece(from_file, from_rank, &conf);
        int *to_pos = getBoardPosition(to_file, to_rank, &conf);

        if (pc == NULL || to_pos == NULL)
        {
            break;
        }

        if ((pieceMoved = movePiece(pc, to_pos, &conf)))
        {
            printBoard(&conf);
        }
        else
        {
            printf("invalid move\n");
            pieceMoved = 0;
        }

        getchar(); // discard newline from input
        if (pieceMoved == 1)
        {
            pieceMoved = cpuMove(&conf);
        }
    }

    printf("exit.. (no piece moved)\n");

    return 0;
}

int cpuMove(struct config *conf)
{
    struct move next_move = calculateMove(conf, 2);
    movePiece(next_move.p, next_move.to_position, conf);
    printf("cpu move...\n");
    printBoard(conf);
    return 1;
}

int isValidMove(struct config *conf, int xfrom, int yfrom, int xto, int yto)
{
    int result = 1;
    int piece = conf->board[yfrom][xfrom];
    int xmove = abs(xfrom - xto);
    int ymove = abs(yfrom - yto);

    int to_pos = conf->board[yto][xto];
    if (to_pos != 0)
    {
        if (piece < 10 && to_pos < 10)
            return 0;
        if (piece > 10 && to_pos > 10)
            return 0;
    }

    if (piece % 10 == 1)
    {
        if (xmove > 1)
        {
            result = 0;
        }
        if (xmove == 1 && conf->board[yto][xto] == 0)
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
        return isValidBishopMove(xmove, ymove);
    }
    else if (piece % 10 == 5)
    {
        return isValidRookMove(xmove, ymove);
    }
    else if (piece % 10 == 6)
    {
        if (xmove == 0 && ymove && 0)
            return 0;
        if (xmove > 1 || ymove > 1)
            return 0;
        return 1;
    }
    else if (piece % 10 == 9)
    {
        return isValidQueenMove(xmove, ymove);
    }
    else
    {
        result = 0;
    }
    return result;
}

int isValidBishopMove(int xmove, int ymove)
{
    if (xmove != ymove)
        return 0;
    return 1;
}

int isValidRookMove(int xmove, int ymove)
{
    if (xmove == 0 || ymove == 0)
        return 1;
    return 0;
}

int isValidQueenMove(int xmove, int ymove)
{
    return isValidBishopMove(xmove, ymove) || isValidRookMove(xmove, ymove);
}

int abs(int x)
{
    if (x < 0)
        return x * -1;
    return x;
}

int movePiece(struct piece *p, int *to, struct config *conf)
{
    if (p != NULL)
    {
        for (int i = 0; i < sizeof(p->available_positions) / sizeof(p->available_positions[0]); i++)
        {
            if (p->available_positions[i] == to)
            {
                *to = *p->current_position;
                *p->current_position = 0;
                p->current_position = to;

                update_available_positions(conf);
                return 1;
            }
        }
    }
    return 0;
}

void update_available_positions(struct config *conf)
{
    for (int i = 0; i < sizeof(conf->white) / sizeof(conf->white[0]); i++)
    {
        if (conf->white[i].type != 0)
            determine_available_positions(&conf->white[i], conf);
    }
    for (int i = 0; i < sizeof(conf->black) / sizeof(conf->black[0]); i++)
    {
        if (conf->black[i].type != 0)
            determine_available_positions(&conf->black[i], conf);
    }
    printf("all pieces updated\n");
}

void determine_available_positions(struct piece *p, struct config *conf)
{
    int valid_pos_counter = 0;
    int piece_file = 0;
    int piece_rank = 0;
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            if (conf->board[x][y] == *p->current_position)
            {
                piece_file = x;
                piece_rank = y;
                goto found;
            }
        }
    }
found:
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            if (isValidMove(conf, piece_rank, piece_file, y, x))
            {
                p->available_positions[valid_pos_counter] = &conf->board[x][y];
                valid_pos_counter++;
            }
        }
    }
}

int *getBoardPosition(int rank, int file, struct config *conf)
{
    if (rank < 0 || rank > BOARD_SIZE)
    {
        return NULL;
    }
    if (file < 0 || file > BOARD_SIZE)
    {
        return NULL;
    }
    return &conf->board[file][rank];
}

struct piece *getPiece(int rank, int file, struct config *conf)
{
    for (int i = 0; i < sizeof(conf->white) / sizeof(conf->white[0]); i++)
    {
        if (conf->white[i].current_position == &conf->board[file][rank])
        {
            return &conf->white[i];
        }
    }
    return NULL;
}

void printBoard(struct config *conf)
{
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        printSolidLine();
        printIntermediateLine();
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            printPiece(conf->board[x][y]);
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