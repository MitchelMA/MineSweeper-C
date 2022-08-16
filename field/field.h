#ifndef __FIELD_H__
#define __FIELD_H__

#define IS_UNOPENED_MASK 0x0000 // 0: 0b0000
#define IS_OPEN_MASK 0x0001     // 1: 0b0001
#define IS_BOMB_MASK 0x0002     // 2: 0b0010
#define IS_FLAGGED_MASK 0x0004  // 4: 0b0100

#include <stdbool.h>
#include <stdint.h>

typedef struct _field Field;
typedef struct _cell Cell;

struct _field
{
    bool gameover;
    uint32_t bombper;
    uint32_t seed;
    size_t size;
    size_t caretx;
    size_t carety;
    Cell **cells;
};

struct _cell
{
    uint32_t status : 3;
    uint32_t bombneighbours : 4;
};

int init_field(Field *field);
void print_field(const Field *field);

int open_cell(Cell *cell);
void open_neighbour(Field *field, size_t x, size_t y);
int flag_cell(Cell *cell);
int eval_field(const Field *field);
void open_field(Field *field);

int is_open(const Cell *cell);
int is_bomb(const Cell *cell);
int is_flagged(const Cell *cell);

#endif // __FIELD_H__