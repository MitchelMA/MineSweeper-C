#include <stdlib.h>
#include <stdio.h>
// for rng
#include <time.h>
#include <conio.h>

#include "field.h"

#define ANSI_RESET "\033[m"

int init_field(Field *field, unsigned int bombpercentage)
{
    if (field == NULL)
    {
        return 0;
    }

    field->caretx = 0;
    field->carety = 0;
    field->gameover = false;
    int cellamount = FIELD_SIZE * FIELD_SIZE;
    int bombamount = ((float)bombpercentage / (float)100) * cellamount;
    if (bombamount == 0)
    {
        return 0;
    }
    field->bombamount = bombamount;

    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; i++)
    {
        int x = i % FIELD_SIZE;
        int y = i / FIELD_SIZE;
        Cell *cell = malloc(sizeof(cell));
        cell->bombneighbours = 0;
        cell->isbomb = false;
        cell->isflagged = false;
        cell->isopened = false;
        field->cells[y][x] = cell;
    }

    time_t t1;
    long long seed = time(&t1);
    srand(seed);

    int times = 0;
    for (int i = 0; i < bombamount;)
    {
        times++;
        int randx = rand() % FIELD_SIZE;
        int randy = rand() % FIELD_SIZE;

        Cell *cell = field->cells[randy][randx];

        if (cell->isbomb)
        {
            continue;
        }

        cell->isbomb = true;
        if (randx > 0)
        {
            field->cells[randy][randx - 1]->bombneighbours++;
        }
        if (randx < FIELD_SIZE - 1)
        {
            field->cells[randy][randx + 1]->bombneighbours++;
        }

        if (randy > 0)
        {
            field->cells[randy - 1][randx]->bombneighbours++;
        }
        if (randy < FIELD_SIZE - 1)
        {
            field->cells[randy + 1][randx]->bombneighbours++;
        }

        if (randx > 0 && randy > 0)
        {
            field->cells[randy - 1][randx - 1]->bombneighbours++;
        }
        if (randx < FIELD_SIZE - 1 && randy > 0)
        {
            field->cells[randy - 1][randx + 1]->bombneighbours++;
        }
        if (randx > 0 && randy < FIELD_SIZE - 1)
        {
            field->cells[randy + 1][randx - 1]->bombneighbours++;
        }
        if (randx < FIELD_SIZE - 1 && randy < FIELD_SIZE - 1)
        {
            field->cells[randy + 1][randx + 1]->bombneighbours++;
        }
        i++;
    }

    return 1;
}

void print_field(const Field *field)
{
    char *ANSI_COLORS[9] = {"\033[0m", "\033[34m", "\033[32m", "\033[31m", "\033[35m", "\033[90m", "\033[36m", "\033[2m", "\033[33m"};

    for (int y = 0; y < FIELD_SIZE; y++)
    {
        for (int x = 0; x < FIELD_SIZE; x++)
        {
            bool chosen = x == field->caretx && y == field->carety;
            Cell *cell = field->cells[y][x];
            printf("%c", chosen ? '[' : ' ');
            if (!cell->isopened)
            {
                printf(".");
                if (cell->isflagged)
                {
                    printf("\033[1D\033[4m\033[97m\033[101mf" ANSI_RESET);
                }
                if (field->gameover && cell->isbomb)
                {
                    printf("\033[1D\033[30m\033[41m*" ANSI_RESET);
                }
            }
            else if (cell->isbomb)
            {
                printf("\033[30m\033[41m*" ANSI_RESET);
            }
            else
            {
                printf("%s%c" ANSI_RESET, ANSI_COLORS[cell->bombneighbours], cell->bombneighbours == 0 ? ' ' : cell->bombneighbours + 48);
            }
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

void open_neighbour(Field *field, unsigned int x, unsigned int y)
{
    if (field == NULL || x >= FIELD_SIZE || y >= FIELD_SIZE || x < 0 || y < 0)
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

int eval_field(Field *field)
{
    for (int y = 0; y < FIELD_SIZE; y++)
    {
        for (int x = 0; x < FIELD_SIZE; x++)
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
    for (int y = 0; y < FIELD_SIZE; y++)
    {
        for (int x = 0; x < FIELD_SIZE; x++)
        {
            field->cells[y][x]->isopened = true;
        }
    }
}