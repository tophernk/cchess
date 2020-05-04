#ifndef CCHESS_POSITION_H
#define CCHESS_POSITION_H

typedef struct position position_t;

position_t *position_new();

void position_ctor(position_t *);

void position_dtor(position_t *);

void position_copy(position_t *src, position_t *dst);

int position_get_x(position_t *);

void position_set_x(position_t *, int x);

int position_get_y(position_t *);

void position_set_y(position_t *, int y);

int position_equal(position_t *, position_t *);

int position_valid(char *);

void position_invalidate(position_t *position);

int position_get_x_(char file);

int position_get_y_(char rank);

char position_get_file(int x);

char position_get_rank(int y);

void position_init(char *);

#endif //CCHESS_POSITION_H
