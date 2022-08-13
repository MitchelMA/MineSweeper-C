#ifndef __FIELD_H__
#define __FIELD_H__

#define IS_UNOPENED_MASK 0x0000 // 0
#define IS_OPEN_MASK 0x0001     // 1
#define IS_BOMB_MASK 0x0002     // 2
#define IS_FLAGGED_MASK 0x0004  // 4

#include <stdbool.h>
#include <stdint.h>

typedef struct _field Field;
typedef struct _cell Cell;

struct _field
{
    bool gameover;
    Cell **cells;
    int bombper;
    uint32_t seed;
    size_t size;
    size_t caretx;
    size_t carety;
};

struct _cell
{
    uint32_t bombneighbours : 4;
    uint32_t status;
};

int init_field(Field *field, size_t fieldsize, int bombpercentage, uint32_t *seed, uint32_t ***mask);
void print_field(const Field *field);

int open_cell(Cell *cell);
void open_neighbour(Field *field, size_t x, size_t y);
int flag_cell(Cell *cell);
int eval_field(const Field *field);
void open_field(Field *field);
int field_masks(const Field *field, uint32_t ***out_masks);

int is_open(const Cell *cell);
int is_bomb(const Cell *cell);
int is_flagged(const Cell *cell);

#endif // __FIELD_H__