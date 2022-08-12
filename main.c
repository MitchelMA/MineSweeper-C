#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include "field/field.h"
#include "input/input.h"
#include "saveio/saveio.h"

uint32_t standfieldsize = 30;
int standbombper = 10;
uint32_t seed = 0;

int alloc_masks(uint32_t ***mask, uint32_t size);

int main(int argc, char *argv[])
{
    uint32_t **masks = NULL;
    int rstatus = 0;
    if (argc == 1)
    {
        rstatus = read_save(&standfieldsize, &standbombper, &seed, &masks);
    }
    // field size
    if (argc > 1)
    {
        clear_save();
        argv++;
        standfieldsize = atoi(*argv);
    }

    // bomb-percentage
    if (argc > 2)
    {
        argv++;
        standbombper = atoi(*argv);
    }

    // seed
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

    for (uint32_t y = 0; y < myfield.size; y++)
    {
        for (uint32_t x = 0; x < myfield.size; x++)
        {
            free(myfield.cells[y][x]);
        }
        free(myfield.cells[y]);
    }
    free(myfield.cells);

    // free the masks
    for (uint32_t i = 0; i < myfield.size; i++)
    {
        free(masks[i]);
    }
    free(masks);

    return EXIT_SUCCESS;
}

int alloc_masks(uint32_t ***masks, uint32_t size)
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
    for (uint32_t y = 0; y < size; y++)
    {
        arr[y] = malloc(sizeof(int) * size);
        if (arr[y] == NULL)
        {
            return 0;
        }
        for (uint32_t x = 0; x < size; x++)
        {
            arr[y][x] = IS_UNOPENED_MASK;
        }
    }

    *masks = arr;
    return 1;
}