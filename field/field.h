#ifndef __FIELD_H__
#define __FIELD_H__
#include <stdbool.h>
#include <stdint.h>

#define FIELD_SIZE 30

typedef struct _field Field;
typedef struct _cell Cell;

struct _field
{
    bool gameover;
    int fieldsize;
    int caretx;
    int carety;
    Cell *cells[FIELD_SIZE][FIELD_SIZE];
};

struct _cell
{
    bool isbomb;
    bool isflagged;
    bool isopened;
    int bombneighbours : 4;
};

int init_field(Field *field, uint32_t fieldsize, uint32_t bombpercentage);
void print_field(const Field *field);

int open_cell(Cell *cell);
void open_neighbour(Field *field, uint32_t x, uint32_t y);
int flag_cell(Cell *cell);
int eval_field(Field *field);
void open_field(Field *field);

#endif // __FIELD_H__