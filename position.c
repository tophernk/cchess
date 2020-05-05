#include <stdlib.h>
#include "position.h"

int position_equal(char *a, char *b) {
    return a[0] == b[0] && a[1] == b[1];
}

int position_valid(char *position) {
    return position[0] != '-' && position[1] != '-';
}

void position_copy(char*src, char*dst) {
    dst[0] = src[0];
    dst[1] = src[1];
}

int position_get_x(char file) {
    return file - 'a';
}

int position_get_y(char rank) {
    return (rank - '8') * -1;
}

char position_get_file(int x) {
    return x + 'a';
}

char position_get_rank(int y) {
    return y * -1 + '8';
}

void position_invalidate(char *position) {
    position[0] = '-';
    position[1] = '-';
}
