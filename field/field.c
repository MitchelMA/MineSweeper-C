#include <stdlib.h>
#include <stdio.h>
// for rng
#include <time.h>
#include <conio.h>

#include "field.h"

#ifdef _PRETTY
#define ANSI_RESET "\033[m"
#endif // _PRETTY

// LOCAL PROTOTYPES
void cell_set_bomb(Field *field, int x, int y);

int init_field(Field *field, uint32_t fieldsize, uint32_t bombpercentage, unsigned int *seed, int **masks)
{
    if (field == NULL || fieldsize < 4 || bombpercentage >= 100)
    {
        return 0;
    }

    field->caretx = 0;
    field->carety = 0;
    field->gameover = false;
    field->size = fieldsize;
    int cellamount = field->size * field->size;
    int bombamount = ((float)bombpercentage / (float)100) * cellamount;
    if (bombamount == 0)
    {
        return 0;
    }

    // allocate memory for the 2d cells array
    field->cells = malloc(sizeof(Cell **) * field->size);
    for (int i = 0; i < field->size; i++)
    {
        field->cells[i] = malloc(sizeof(Cell *) * field->size);
    }

    for (int i = 0; i < field->size * field->size; i++)
    {
        int x = i % field->size;
        int y = i / field->size;
        Cell *cell = malloc(sizeof(cell));
        if (cell == NULL)
        {
            return 0;
        }
        cell->bombneighbours = 0;
        cell->isbomb = false;
        cell->isflagged = false;
        cell->isopened = false;
        field->cells[y][x] = cell;
    }

    time_t t1;
    unsigned int s = time(&t1);
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

    if (masks == NULL)
    {

        int times = 0;
        for (int i = 0; i < bombamount;)
        {
            times++;
            int randx = rand() % field->size;
            int randy = rand() % field->size;

            Cell *cell = field->cells[randy][randx];

            if (cell->isbomb)
            {
                continue;
            }

            cell_set_bomb(field, randx, randy);
            i++;
        }
    }
    else
    {
        int times = 0;
        for (int i = 0; i < field->size * field->size; i++)
        {
            int x = i % field->size;
            int y = i / field->size;

            Cell *curcell = field->cells[y][x];
            int curmask = masks[y][x];

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
    return 1;
}

void print_field(const Field *field)
{
#ifdef _PRETTY
    char *ANSI_COLORS[9] = {"\033[0m", "\033[34m", "\033[32m", "\033[31m", "\033[35m", "\033[90m", "\033[36m", "\033[2m", "\033[33m"};
#endif // _PRETTY

    for (int y = 0; y < field->size; y++)
    {
        for (int x = 0; x < field->size; x++)
        {
            bool chosen = x == field->caretx && y == field->carety;
            Cell *cell = field->cells[y][x];
            printf("%c", chosen ? '[' : ' ');
#ifdef _PRETTY
            if (!cell->isopened)
            {
                printf(".");
                if (cell->isflagged)
                {
                    printf("\033[1D\033[4m\033[97m\033[101mf" ANSI_RESET);
                }
                if (field->gameover && cell->isbomb)
                {
                    printf("\033[1D\033[97m\033[41m*" ANSI_RESET);
                }
            }
            else if (cell->isbomb)
            {
                printf("\033[97m\033[41m*" ANSI_RESET);
            }
            else
            {
                printf("%s%c" ANSI_RESET, ANSI_COLORS[cell->bombneighbours], cell->bombneighbours == 0 ? ' ' : cell->bombneighbours + 48);
            }
#else
            if (cell->isflagged)
            {
                printf("f");
            }
            else if (cell->isopened)
            {
                if (cell->isbomb)
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
    if (cell == NULL || cell->isflagged || cell->isopened)
    {
        return 0;
    }

    cell->isopened = true;
    return 1;
}

int flag_cell(Cell *cell)
{
    // situations in which a cell cannot be flagged
    if (cell == NULL || cell->isopened)
    {
        return 0;
    }

    cell->isflagged = !cell->isflagged;
    return 1;
}

void open_neighbour(Field *field, uint32_t x, uint32_t y)
{
    if (field == NULL || x >= field->size || y >= field->size || x < 0 || y < 0)
    {
        return;
    }

    // open this one, then recursively open neighbouring cells
    Cell *curcell = field->cells[y][x];
    if (curcell->isflagged)
    {
        return;
    }
    if (curcell->isopened)
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
    for (int y = 0; y < field->size; y++)
    {
        for (int x = 0; x < field->size; x++)
        {
            Cell *curcell = field->cells[y][x];
            if (curcell->isbomb && !curcell->isflagged)
            {
                return 0;
            }
            if (!curcell->isbomb && !curcell->isopened)
            {
                return 0;
            }
        }
    }

    return 1;
}

void open_field(Field *field)
{
    for (int y = 0; y < field->size; y++)
    {
        for (int x = 0; x < field->size; x++)
        {
            field->cells[y][x]->isopened = true;
        }
    }
}

int field_masks(const Field *field, int ***out_masks)
{
    if (field == NULL || out_masks == NULL)
    {
        printf("out-mask or field was NULL\n");
        return 0;
    }

    for (int y = 0; y < field->size; y++)
    {
        for (int x = 0; x < field->size; x++)
        {
            int value = 0;
            Cell *curcell = field->cells[y][x];
            if (curcell->isopened)
            {
                value |= IS_OPEN_MASK;
            }

            if (curcell->isbomb)
            {
                value |= IS_BOMB_MASK;
            }

            if (curcell->isflagged)
            {
                value |= IS_FLAGGED_MASK;
            }

            (*out_masks)[y][x] = value;
        }
    }
    return 1;
}

void cell_set_bomb(Field *field, int x, int y)
{

    Cell *curcell = field->cells[y][x];

    curcell->isbomb = true;
    if (x > 0)
    {
        field->cells[y][x - 1]->bombneighbours++;
    }
    if (x < field->size - 1)
    {
        field->cells[y][x + 1]->bombneighbours++;
    }

    if (y > 0)
    {
        field->cells[y - 1][x]->bombneighbours++;
    }
    if (y < field->size - 1)
    {
        field->cells[y + 1][x]->bombneighbours++;
    }

    if (x > 0 && y > 0)
    {
        field->cells[y - 1][x - 1]->bombneighbours++;
    }
    if (x < field->size - 1 && y > 0)
    {
        field->cells[y - 1][x + 1]->bombneighbours++;
    }
    if (x > 0 && y < field->size - 1)
    {
        field->cells[y + 1][x - 1]->bombneighbours++;
    }
    if (x < field->size - 1 && y < field->size - 1)
    {
        field->cells[y + 1][x + 1]->bombneighbours++;
    }
}