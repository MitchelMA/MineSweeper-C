#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "field/field.h"
#include "input/input.h"
#include "saveio/saveio.h"

int handle_open(Field *field);
void handle_arrows(Field *field, int arrow);

// int main()
// {
//     Field field = {0};
//     bin_read(&field);
//     for (size_t y = 0; y < field.size; y++)
//     {
//         for (size_t x = 0; x < field.size; x++)
//         {
//             printf("(%u : %u) ", field.cells[y][x].bombneighbours, field.cells[y][x].status);
//         }
//         printf("\n");
//     }
// }

int main2(int argc, char *argv[])
{
    Field myfield = {0};
    if (argc == 1)
    {
        bin_read(&myfield);
    }
    // field size
    if (argc > 1)
    {
        argv++;
        myfield.size = atoi(*argv);
    }

    // bomb-percentage
    if (argc > 2)
    {
        argv++;
        myfield.bombper = atoi(*argv);
    }

    // seed
    if (argc > 3)
    {
        argv++;
        myfield.seed = atoi(*argv);
    }

    if (!init_field(&myfield))
    {
        printf("Het lukte niet om het speelveld te initializeren\n");
        return EXIT_FAILURE;
    }
    printf("seed: %d\n", myfield.seed);

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
            if (!handle_open(&myfield))
            {
                myfield.gameover = true;
#ifndef _PRETTY
                clrscrn();
#endif // _PRETTY
                open_field(&myfield);
                print_field(&myfield);
                printf("Game Over!\nJe opende een bom!\n");
                bin_write(&myfield, 0);
                break;
            }

            goto evaluation;
        }
        // flagging
        if (input == 102)
        {
            Cell *curcell = &myfield.cells[myfield.carety][myfield.caretx];
            flag_cell(curcell);

            goto evaluation;
        }

        // esc code
        if (input == 27 || input == 113)
        {
            // field_masks(&myfield, &masks);
            bin_write(&myfield, 1);
            break;
        }

        // arrow-keys
        if (arrow)
        {
            handle_arrows(&myfield, arrow);
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
            bin_write(&myfield, 0);
            break;
        }
    }

    // free the cells
    for (uint32_t y = 0; y < myfield.size; y++)
    {
        free(myfield.cells[y]);
    }
    free(myfield.cells);

    return EXIT_SUCCESS;
}

int handle_open(Field *field)
{
    Cell *curcell = &field->cells[field->carety][field->caretx];
    if (curcell->bombneighbours == 0)
    {
        open_neighbour(field, field->caretx, field->carety);
    }
    else
    {
        open_cell(curcell);
    }

    // game-over
    if (is_bomb(curcell) && is_open(curcell))
    {

        return 0;
    }

    return 1;
}

void handle_arrows(Field *field, int arrow)
{
    switch (arrow)
    {
    // up
    case 'H':
    {
        if (field->carety > 0)
            field->carety--;
        break;
    }

    // right
    case 'M':
    {
        if (field->caretx < field->size - 1)
            field->caretx++;
        break;
    }

    // down
    case 'P':
    {
        if (field->carety < field->size - 1)
            field->carety++;
        break;
    }

    // left
    case 'K':
    {
        if (field->caretx > 0)
            field->caretx--;
        break;
    }
    }
}