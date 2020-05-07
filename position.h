#ifndef CCHESS_POSITION_H
#define CCHESS_POSITION_H

void position_copy(char *src, char *dst);

int position_equal(char *, char *);

int position_valid(char *);

int position_to_x(char file);

int position_to_y(char rank);

char position_to_file(int x);

char position_to_rank(int y);

int position_get_x(char *);

int position_get_y(char *);

void position_invalidate(char *);

void position_set_file_rank(char *, int, int);

#endif //CCHESS_POSITION_H
