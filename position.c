#include <stdlib.h>
#include "position.h"

struct position {
    int x;
    int y;
};

position_t* position_new() {
       return (position_t *) malloc(sizeof(position_t));
}

void position_ctor(position_t *position) {
   position->x = -1;
   position->y = -1;
}

void position_dtor(position_t *position) {
    // nothing to do
}

void position_invalidate(position_t *position) {
    position_ctor(position);
}

int position_equal(position_t *a, position_t *b) {
   return a->x == b->x && a->y == b->y;
}

int position_valid(position_t *position) {
   return position->x != -1 && position->y != -1;
}

int position_get_x(position_t *position) {
    return position->x;
}

void position_set_x(position_t *position, int x) {
    position->x = x;
}

int position_get_y(position_t *position) {
    return position->y;
}

void position_set_y(position_t *position, int y) {
    position->y = y;
}

void position_copy(position_t *src, position_t *dst) {
    dst->x = src->x;
    dst->y = src->y;
}
