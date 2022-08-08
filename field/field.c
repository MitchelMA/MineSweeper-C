#include <stdlib.h>
#include <stdio.h>
// for rng
#include <time.h>
#include <conio.h>

#include "field.h"

int init_field(Field *field, unsigned int bombpercentage)
{
    if (field == NULL)
    {
        return 0;
    }

    field->caretx = 0;
    field->carety = 0;
    int cellamount = FIELD_SIZE * FIELD_SIZE;
    int bombamount = ((float)bombpercentage / (float)100) * cellamount;
    if (bombamount == 0)
    {
        return 0;
    }

    for (int y = 0; y < FIELD_SIZE; y++)
    {
        for (int x = 0; x < FIELD_SIZE; x++)
        {
            Cell *cell = malloc(sizeof(cell));
            cell->bombneighbours = 0;
            cell->isbomb = false;
            cell->isflagged = false;
            cell->isopened = false;
            field->cells[y][x] = cell;
        }
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
        // printf(
        //     "cell-status:\n\t"
        //     "is-bomb: %d\n\t"
        //     "is-flagged: %d\n\t"
        //     "is-opened: %d\n\t"
        //     "bomb-neighbours: %d\n\t"
        //     "address: %p\n",
        //     cell->isbomb, cell->isflagged, cell->isopened, cell->bombneighbours, cell);

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
    for (int y = 0; y < FIELD_SIZE; y++)
    {
        for (int x = 0; x < FIELD_SIZE; x++)
        {
            bool chosen = x == field->caretx && y == field->carety;
            printf("%c", chosen ? '[' : ' ');
            printf(".");
            printf("%c", chosen ? ']' : ' ');
        }
        printf("\n");
    }
}