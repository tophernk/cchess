#ifndef CCHESS_POSITION_H
#define CCHESS_POSITION_H

typedef struct position position_t;

position_t* position_new();

void position_ctor(position_t*);

void position_dtor(position_t*);

int position_get_x(position_t*);

void position_set_x(position_t*, int x);

int position_get_y(position_t*);

void position_set_y(position_t*, int y);

int position_equal(position_t*, position_t*);

int position_valid(position_t*);

void position_invalidate(position_t *position);

#endif //CCHESS_POSITION_H
