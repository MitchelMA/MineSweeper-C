#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "field/field.h"
#include "input/input.h"
#include "saveio/saveio.h"

int standfieldsize = 30;
int standbombper = 10;
unsigned int seed = 0;

// int write_save();
// int read_save();

int alloc_masks(int ***mask, int size);

int main(int argc, char *argv[])
{
    int **masks = NULL;
    int rstatus = 0;
    if (argc == 1)
    {
        rstatus = read_save(&standfieldsize, &standbombper, &seed, &masks);
    }
    if (argc > 1)
    {
        clear_save();
        argv++;
        standfieldsize = atoi(*argv);
    }
    if (argc > 2)
    {
        argv++;
        standbombper = atoi(*argv);
    }

    if (argc > 3)
    {
        argv++;
        seed = atoi(*argv);
    }

    Field myfield;
    if (!init_field(&myfield, standfieldsize, standbombper, &seed, masks))
    {
        printf("Het lukte niet om het speelveld te initializeren\n");
        return EXIT_FAILURE;
    }

    if (!rstatus || rstatus == 2)
    {

        if (!alloc_masks(&masks, standfieldsize))
        {
            printf("Kon geen geheugen vragen voor masks\n");
            return EXIT_FAILURE;
        }
    }
    printf("seed: %d\n", seed);

    int input = 0;

    while (1)
    {
#ifndef _PRETTY
        clrscrn();
#endif // _PRETTY
       // cursor movement
        print_field(&myfield);
#ifdef _PRETTY
        printf("\033[%dA", myfield.size);
#endif // _PRETTY
        int arrow = get_arrow_keys(&input);

        // enter or space press
        if (input == 13 || input == 32)
        {
            Cell *curcell = myfield.cells[myfield.carety][myfield.caretx];
            if (curcell->bombneighbours == 0)
            {
                open_neighbour(&myfield, myfield.caretx, myfield.carety);
            }
            else
            {
                open_cell(curcell);
            }

            // game-over
            if (curcell->isbomb && curcell->isopened)
            {
                myfield.gameover = true;
#ifndef _PRETTY
                clrscrn();
#endif // _PRETTY
                open_field(&myfield);
                print_field(&myfield);
                printf("Game Over!\nJe opende een bom!\n");
                write_save(myfield.size, standbombper, 0, seed, masks);
                break;
            }

            goto evaluation;
        }
        // flagging
        if (input == 102)
        {
            Cell *curcell = myfield.cells[myfield.carety][myfield.caretx];
            flag_cell(curcell);

            goto evaluation;
        }

        // esc code
        if (input == 27 || input == 113)
        {

            field_masks(&myfield, &masks);

            write_save(myfield.size, standbombper, 1, seed, masks);
            break;
        }

        // arrow-keys
        if (arrow)
        {
            switch (arrow)
            {
            // up
            case 'H':
            {
                if (myfield.carety > 0)
                    myfield.carety--;
                break;
            }

            // right
            case 'M':
            {
                if (myfield.caretx < myfield.size - 1)
                    myfield.caretx++;
                break;
            }

            // down
            case 'P':
            {
                if (myfield.carety < myfield.size - 1)
                    myfield.carety++;
                break;
            }

            // left
            case 'K':
            {
                if (myfield.caretx > 0)
                    myfield.caretx--;
                break;
            }
            }
        }
    evaluation:
        // evaluate the field at the end of every
        if (eval_field(&myfield))
        {
#ifndef _PRETTY
            clrscrn();
#endif // _PRETTY
            open_field(&myfield);
            print_field(&myfield);
            printf("Je hebt gewonnen!\n");
            write_save(myfield.size, standbombper, 0, seed, masks);
            break;
        }
    }

    for (int y = 0; y < myfield.size; y++)
    {
        for (int x = 0; x < myfield.size; x++)
        {
            free(myfield.cells[y][x]);
        }
        free(myfield.cells[y]);
    }
    free(myfield.cells);

    return EXIT_SUCCESS;
}

int alloc_masks(int ***masks, int size)
{
    if (masks == NULL || size <= 1)
    {
        return 0;
    }

    int **arr = malloc(sizeof(int *) * size);
    if (arr == NULL)
    {
        return 0;
    }
    for (int i = 0; i < size; i++)
    {
        arr[i] = malloc(sizeof(int) * size);
        if (arr[i] == NULL)
        {
            return 0;
        }
    }

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            arr[y][x] = 0;
        }
    }

    *masks = arr;
    return 1;
}