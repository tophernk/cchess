#include <stdio.h>
#include "cchess.h"
#include "config.h"
#include "config_p.h"

void __print_solid_line();

void __print_intermediate_line();

void __print_piece(piece_type_t);

void config_print(config_t *conf) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        __print_solid_line();
        __print_intermediate_line();
        printf("  %d ", 8 - x);
        for (int y = 0; y < BOARD_SIZE; y++) {
            __print_piece(conf->board[y][x]);
        }
        printf("|\n");
        __print_intermediate_line();
    }
    __print_solid_line();
    printf("        A       B       C       D       E       F       G       H    \n");
}

void __print_solid_line() {
    printf("    ");
    for (int i = 0; i < BOARD_SIZE * 8; i++) {
        printf("-");
    }
    printf("-\n");
}

void __print_intermediate_line() {
    printf("    ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("|       ");
    }
    printf("|\n");
}

void __print_piece(piece_type_t type) {
    printf("|   ");
    printf("%c", piece_tpye_to_char(type));

    if (type > QUEEN_W) {
        printf("*  ");
    } else {
        printf("   ");
    }
}
