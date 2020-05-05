#ifndef CCHESS_POSITION_H
#define CCHESS_POSITION_H

void position_copy(char *src, char *dst);

int position_equal(char *, char *);

int position_valid(char *);

int position_get_x(char file);

int position_get_y(char rank);

char position_get_file(int x);

char position_get_rank(int y);

void position_invalidate(char *);

#endif //CCHESS_POSITION_H
