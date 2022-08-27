#include <stdlib.h>
#include <stdio.h>
// for rng
#include <time.h>
#include <conio.h>
#include <string.h>

#include "field.h"

// CELL MASKS

#define IS_UNOPENED_MASK 0x0000 // 0: 0b0000
#define IS_OPEN_MASK 0x0001     // 1: 0b0001
#define IS_BOMB_MASK 0x0002     // 2: 0b0010
#define IS_FLAGGED_MASK 0x0004  // 4: 0b0100

// END CELL MASKS

#ifdef _PRETTY
// WRITE_TO_BUFFER_MACROS

#define ANSI_ESCAPE_TO_BUFFER(BUFFER, START_INDEX) \
    BUFFER[START_INDEX++] = '\x1b';                \
    BUFFER[START_INDEX++] = '[';                   \
    BUFFER[START_INDEX++] = 'm';

#define FLAG_TO_BUFFER(BUFFER, START_INDEX) \
    BUFFER[START_INDEX++] = '\x1b';         \
    BUFFER[START_INDEX++] = '[';            \
    BUFFER[START_INDEX++] = '4';            \
    BUFFER[START_INDEX++] = 'm';            \
    BUFFER[START_INDEX++] = '\x1b';         \
    BUFFER[START_INDEX++] = '[';            \
    BUFFER[START_INDEX++] = '9';            \
    BUFFER[START_INDEX++] = '7';            \
    BUFFER[START_INDEX++] = 'm';            \
    BUFFER[START_INDEX++] = '\x1b';         \
    BUFFER[START_INDEX++] = '[';            \
    BUFFER[START_INDEX++] = '1';            \
    BUFFER[START_INDEX++] = '0';            \
    BUFFER[START_INDEX++] = '1';            \
    BUFFER[START_INDEX++] = 'm';            \
    BUFFER[START_INDEX++] = 'f';            \
    ANSI_ESCAPE_TO_BUFFER(BUFFER, START_INDEX)

#define BOMB_TO_BUFFER(BUFFER, START_INDEX) \
    BUFFER[START_INDEX++] = '\x1b';         \
    BUFFER[START_INDEX++] = '[';            \
    BUFFER[START_INDEX++] = '9';            \
    BUFFER[START_INDEX++] = '7';            \
    BUFFER[START_INDEX++] = 'm';            \
    BUFFER[START_INDEX++] = '\x1b';         \
    BUFFER[START_INDEX++] = '[';            \
    BUFFER[START_INDEX++] = '4';            \
    BUFFER[START_INDEX++] = '1';            \
    BUFFER[START_INDEX++] = 'm';            \
    BUFFER[START_INDEX++] = '*';            \
    ANSI_ESCAPE_TO_BUFFER(BUFFER, START_INDEX)

#define NONBOMB_TO_BUFFER(BUFFER, START_INDEX, NEIGHBOURS, ESCAPE_ARRAY) \
    BUFFER[START_INDEX++] = '\x1b';                                      \
    BUFFER[START_INDEX++] = '[';                                         \
    BUFFER[START_INDEX++] = ESCAPE_ARRAY[NEIGHBOURS][2];                 \
    if (ESCAPE_ARRAY[NEIGHBOURS][3] == 'm')                              \
    {                                                                    \
        BUFFER[START_INDEX++] = 'm';                                     \
    }                                                                    \
    else                                                                 \
    {                                                                    \
        BUFFER[START_INDEX++] = ESCAPE_ARRAY[NEIGHBOURS][3];             \
        BUFFER[START_INDEX++] = 'm';                                     \
    }                                                                    \
    BUFFER[START_INDEX++] = NEIGHBOURS == 0 ? ' ' : NEIGHBOURS + 48;     \
    ANSI_ESCAPE_TO_BUFFER(BUFFER, START_INDEX)

// END OF MACROS

#endif // _PRETTY

// LOCAL PROTOTYPES

int open_cell(Cell *cell);
void cell_set_bomb(Field *field, size_t x, size_t y);

#define STAND_FIELD_SIZE 30
#define STAND_BOMB_PER 10
int init_field(Field *field)
{
    if (field == NULL)
    {
        return 0;
    }

    if (field->size < 4)
    {
        field->size = STAND_FIELD_SIZE;
    }

    if (field->bombper <= 0 || field->bombper > 100)
    {
        field->bombper = STAND_BOMB_PER;
    }
    size_t bombcount = ((float)field->bombper / (float)100) * (field->size * field->size);

    field->seed == 0 ? field->seed = time(0) : false;

    srand(field->seed);

    if (field->cells != NULL)
    {
        return 1;
    }

    // allocate memory for the cells
    field->cells = calloc(field->size, sizeof(Cell *));
    if (field->cells == NULL)
    {
        return 0;
    }

    for (size_t y = 0; y < field->size; y++)
    {
        field->cells[y] = calloc(field->size, sizeof(Cell));
        if (field->cells[y] == NULL)
        {
            return 0;
        }
    }

    for (size_t i = 0; i < bombcount;)
    {
        size_t randx = rand() % field->size;
        size_t randy = rand() % field->size;

        Cell *cell = &field->cells[randy][randx];

        if (is_bomb(cell))
        {
            continue;
        }

        cell_set_bomb(field, randx, randy);
        i++;
    }

    return 1;
}

void print_field(const Field *field)
{
#ifdef _PRETTY
    char *ANSI_COLORS[9] = {"\x1b[0m", "\x1b[34m", "\x1b[32m", "\x1b[31m", "\x1b[35m", "\x1b[90m", "\x1b[36m", "\x1b[2m", "\x1b[33m"};
#endif // _PRETTY

// allocate a buffer to write into so we only have to write once to the stdout
#ifdef _PRETTY
    size_t buffer_size = 21 * field->size * field->size + field->size + 1;
#else
    size_t buffer_size = 3 * field->size * field->size + field->size + 1;
#endif // _PRETTY0
    //                                                   ^ for '\n'    ^ +1 for the null-terminator ('\0')
    char *buffer = calloc(buffer_size, sizeof(char));
    size_t buffer_index = 0;

    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            bool chosen = x == field->caretx && y == field->carety;
            Cell *cell = &field->cells[y][x];
            buffer[buffer_index++] = chosen ? '[' : ' ';
#ifdef _PRETTY
            if (is_flagged(cell))
            {
                FLAG_TO_BUFFER(buffer, buffer_index);
            }
            else if (is_open(cell))
            {
                if (is_bomb(cell))
                {
                    BOMB_TO_BUFFER(buffer, buffer_index);
                }
                else
                {
                    uint32_t neighbour_count = cell->bombneighbours;
                    NONBOMB_TO_BUFFER(buffer, buffer_index, neighbour_count, ANSI_COLORS)
                }
            }
            else
            {
                buffer[buffer_index++] = '.';
            }
#else
            if (is_flagged(cell))
            {
                buffer[buffer_index++] = 'f';
            }
            else if (is_open(cell))
            {
                if (is_bomb(cell))
                {
                    buffer[buffer_index++] = '*';
                }
                else
                {
                    buffer[buffer_index++] = cell->bombneighbours == 0 ? ' ' : cell->bombneighbours + 48;
                }
            }
            else
            {
                buffer[buffer_index++] = '.';
            }
#endif // _PRETTY
            buffer[buffer_index++] = chosen ? ']' : ' ';
        }
        buffer[buffer_index++] = '\n';
    }

    // print the buffer once to the stdout
    printf("%s", buffer);
    // free the memory of the buffer
    free(buffer);
}

int open_cell(Cell *cell)
{
    // situations in which a cell cannot be opened
    if (cell == NULL || is_flagged(cell) || is_open(cell))
    {
        return 0;
    }

    cell->status |= IS_OPEN_MASK;
    return 1;
}

int flag_cell(Cell *cell)
{
    // situations in which a cell cannot be flagged
    if (cell == NULL || is_open(cell))
    {
        return 0;
    }

    cell->status ^= IS_FLAGGED_MASK;
    return 1;
}

void open_series(Field *field, size_t x, size_t y)
{
    if (field == NULL || x >= field->size || y >= field->size || x < 0 || y < 0)
    {
        return;
    }

    // open this one, then recursively open neighbouring cells
    Cell *curcell = &field->cells[y][x];
    if (is_flagged(curcell))
    {
        return;
    }
    if (is_open(curcell))
    {
        return;
    }

    open_cell(curcell);
    if (curcell->bombneighbours > 0)
        return;

    open_series(field, x - 1, y - 1);
    open_series(field, x, y - 1);
    open_series(field, x + 1, y - 1);
    open_series(field, x - 1, y);
    open_series(field, x + 1, y);
    open_series(field, x - 1, y + 1);
    open_series(field, x, y + 1);
    open_series(field, x + 1, y + 1);
}

int eval_field(const Field *field)
{
    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            Cell *curcell = &field->cells[y][x];

            if (is_bomb(curcell) && !is_flagged(curcell))
            {
                return 0;
            }
            if (!is_bomb(curcell) && !is_open(curcell))
            {
                return 0;
            }
        }
    }

    return 1;
}

void open_field(Field *field)
{
    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            field->cells[y][x].status |= IS_OPEN_MASK;
        }
    }
}

void cell_set_bomb(Field *field, size_t x, size_t y)
{
    if (field == NULL || x < 0 || y < 0 || x >= field->size || y >= field->size)
        return;

    Cell *curcell = &field->cells[y][x];

    curcell->status |= IS_BOMB_MASK;
    if (x > 0)
    {
        field->cells[y][x - 1].bombneighbours++;
    }
    if (x < field->size - 1)
    {
        field->cells[y][x + 1].bombneighbours++;
    }

    if (y > 0)
    {
        field->cells[y - 1][x].bombneighbours++;
    }
    if (y < field->size - 1)
    {
        field->cells[y + 1][x].bombneighbours++;
    }

    if (x > 0 && y > 0)
    {
        field->cells[y - 1][x - 1].bombneighbours++;
    }
    if (x < field->size - 1 && y > 0)
    {
        field->cells[y - 1][x + 1].bombneighbours++;
    }
    if (x > 0 && y < field->size - 1)
    {
        field->cells[y + 1][x - 1].bombneighbours++;
    }
    if (x < field->size - 1 && y < field->size - 1)
    {
        field->cells[y + 1][x + 1].bombneighbours++;
    }
}

int is_open(const Cell *cell)
{
    if (cell == NULL)
        return 0;
    return cell->status & IS_OPEN_MASK;
}

int is_bomb(const Cell *cell)
{
    if (cell == NULL)
        return 0;
    return cell->status & IS_BOMB_MASK;
}

int is_flagged(const Cell *cell)
{
    if (cell == NULL)
        return 0;
    return cell->status & IS_FLAGGED_MASK;
}