#include <stdlib.h>
#include <stdio.h>
// for rng
#include <time.h>
#include <conio.h>
#include <string.h>

#include "field.h"

#ifdef _PRETTY
#define ANSI_RESET "\033[m"
#endif // _PRETTY

// LOCAL PROTOTYPES
void cell_set_bomb(Field *field, size_t x, size_t y);
int alloc_masks(uint32_t ***mask, size_t size);
int init_cell(Cell *cell);

int init_field(Field *field, size_t fieldsize, int bombpercentage, uint32_t *seed, uint32_t ***masks)
{
    if (field == NULL || fieldsize < 4 || bombpercentage >= 100)
    {
        return 0;
    }

    field->caretx = 0;
    field->carety = 0;
    field->gameover = false;
    field->size = fieldsize;
    size_t cellamount = field->size * field->size;
    size_t bombamount = ((float)bombpercentage / (float)100) * cellamount;
    if (bombamount == 0)
    {
        return 0;
    }

    // allocate memory for the 2d cells array
    field->cells = malloc(sizeof(Cell **) * field->size);
    for (size_t i = 0; i < field->size; i++)
    {
        field->cells[i] = malloc(sizeof(Cell *) * field->size);
    }

    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            if (!init_cell(&field->cells[y][x]))
            {
                return 0;
            }
        }
    }

    uint32_t s = time(0);
    if (seed == NULL || *seed == 0)
    {
        srand(s);
        if (seed != NULL)
        {
            *seed = s;
        }
    }
    else
    {
        srand(*seed);
    }

    if (masks == NULL || *masks == NULL)
    {
        // allocate memory for the masks
        if (!alloc_masks(masks, field->size))
        {
            return 0;
        }
        int times = 0;
        for (size_t i = 0; i < bombamount;)
        {
            times++;
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
    }
    else
    {
        for (size_t y = 0; y < field->size; y++)
        {
            for (size_t x = 0; x < field->size; x++)
            {
                Cell *curcell = &field->cells[y][x];
                int curmask = (*masks)[y][x];

                if (curmask & IS_OPEN_MASK)
                {
                    open_cell(curcell);
                }

                if (curmask & IS_BOMB_MASK)
                {
                    cell_set_bomb(field, x, y);
                }

                if (curmask & IS_FLAGGED_MASK)
                {
                    flag_cell(curcell);
                }
            }
        }
    }
    return 1;
}

void print_field(const Field *field)
{
#ifdef _PRETTY
    char *ANSI_COLORS[9] = {"\033[0m", "\033[34m", "\033[32m", "\033[31m", "\033[35m", "\033[90m", "\033[36m", "\033[2m", "\033[33m"};
#endif // _PRETTY

    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            bool chosen = x == field->caretx && y == field->carety;
            Cell *cell = &field->cells[y][x];
            printf("%c", chosen ? '[' : ' ');
#ifdef _PRETTY
            if (!is_open(cell))
            {
                printf(".");
                if (is_flagged(cell))
                {
                    printf("\033[1D\033[4m\033[97m\033[101mf" ANSI_RESET);
                }
                if (field->gameover && is_bomb(cell))
                {
                    printf("\033[1D\033[97m\033[41m*" ANSI_RESET);
                }
            }
            else if (is_bomb(cell))
            {
                printf("\033[97m\033[41m*" ANSI_RESET);
            }
            else
            {
                printf("%s%c" ANSI_RESET, ANSI_COLORS[cell->bombneighbours], cell->bombneighbours == 0 ? ' ' : cell->bombneighbours + 48);
            }
#else
            if (is_flagged(cell))
            {
                printf("f");
            }
            else if (is_open(cell))
            {
                if (is_bomb(cell))
                {
                    printf("*");
                }
                else
                {
                    printf("%c", cell->bombneighbours == 0 ? ' ' : cell->bombneighbours + 48);
                }
            }
            else
            {
                printf(".");
            }
#endif // _PRETTY
            printf("%c", chosen ? ']' : ' ');
        }
        printf("\n");
    }
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

void open_neighbour(Field *field, size_t x, size_t y)
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

    open_neighbour(field, x - 1, y - 1);
    open_neighbour(field, x, y - 1);
    open_neighbour(field, x + 1, y - 1);
    open_neighbour(field, x - 1, y);
    open_neighbour(field, x + 1, y);
    open_neighbour(field, x - 1, y + 1);
    open_neighbour(field, x, y + 1);
    open_neighbour(field, x + 1, y + 1);
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

int field_masks(const Field *field, uint32_t ***out_masks)
{
    if (field == NULL || out_masks == NULL || *out_masks == NULL)
    {
        printf("out-mask or field was NULL\n");
        return 0;
    }

    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            Cell *curcell = &field->cells[y][x];

            (*out_masks)[y][x] = curcell->status;
        }
    }
    return 1;
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

int alloc_masks(uint32_t ***masks, size_t size)
{
    if (masks == NULL || size <= 1)
    {
        return 0;
    }

    uint32_t **arr = malloc(sizeof(int *) * size);
    if (arr == NULL)
    {
        return 0;
    }
    for (size_t y = 0; y < size; y++)
    {
        arr[y] = malloc(sizeof(int) * size);
        if (arr[y] == NULL)
        {
            return 0;
        }
        for (size_t x = 0; x < size; x++)
        {
            arr[y][x] = IS_UNOPENED_MASK;
        }
    }

    *masks = arr;
    return 1;
}

int init_cell(Cell *cell)
{
    memset(cell, 0, sizeof(Cell));

    return 1;
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