#ifndef __FIELD_H__
#define __FIELD_H__
#include <stdbool.h>

#define FIELD_SIZE 8

typedef struct _field Field;
typedef struct _cell Cell;

struct _field
{
    Cell *cells[FIELD_SIZE][FIELD_SIZE];
    int caretx;
    int carety;
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
int flag_cell(Cell *cell);

#endif // __FIELD_H__