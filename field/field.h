#ifndef __FIELD_H__
#define __FIELD_H__
#include <stdbool.h>

#define FIELD_SIZE 30

typedef struct _field Field;
typedef struct _cell Cell;

struct _field
{
    Cell *cells[FIELD_SIZE][FIELD_SIZE];
    int caretx;
    int carety;
    bool gameover;
};

struct _cell
{
    bool isbomb;
    bool isflagged;
    bool isopened;
    int bombneighbours : 4;
};

int init_field(Field *field, unsigned int bombpercentage);
void print_field(const Field *field);

int open_cell(Cell *cell);
void open_neighbour(Field *field, unsigned int x, unsigned int y);
int flag_cell(Cell *cell);
int eval_field(Field *field);
void open_field(Field *field);

#endif // __FIELD_H__