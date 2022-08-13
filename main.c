#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include "field/field.h"
#include "input/input.h"
#include "saveio/saveio.h"

static size_t standfieldsize = 30;
static int standbombper = 10;
static uint32_t seed = 0;

int handle_open(Field *field);
void handle_arrows(Field *field, int arrow);

int main(void)
{
    uint32_t **masks;
    Field myfield;
    memset(&myfield, 0, sizeof(Field));
    printf("size: %d\n", myfield.size);
    read_save(&standfieldsize, &standbombper, &seed, &masks);
    init_field(&myfield, standfieldsize, standbombper, &seed, &masks);
    printf("size: %zu; bomb-percentage: %i\n", standfieldsize, standbombper);
    print_field(&myfield);
    return 0;
}

// int main2(int argc, char *argv[])
// {
//     uint32_t **masks = NULL;
//     int rstatus = 0;
//     if (argc == 1)
//     {
//         rstatus = read_save(&standfieldsize, &standbombper, &seed, &masks);
//     }
//     // field size
//     if (argc > 1)
//     {
//         clear_save();
//         argv++;
//         standfieldsize = atoi(*argv);
//     }

//     // bomb-percentage
//     if (argc > 2)
//     {
//         argv++;
//         standbombper = atoi(*argv);
//     }

//     // seed
//     if (argc > 3)
//     {
//         argv++;
//         seed = atoi(*argv);
//     }

//     Field myfield;
//     if (!init_field(&myfield, standfieldsize, standbombper, &seed, &masks))
//     {
//         printf("Het lukte niet om het speelveld te initializeren\n");
//         return EXIT_FAILURE;
//     }
//     printf("seed: %d\n", seed);

//     int input = 0;

//     while (1)
//     {
// #ifndef _PRETTY
//         clrscrn();
// #endif // _PRETTY
//        // cursor movement
//         print_field(&myfield);
// #ifdef _PRETTY
//         printf("\033[%dA", myfield.size);
// #endif // _PRETTY
//         int arrow = get_arrow_keys(&input);

//         // enter or space press
//         if (input == 13 || input == 32)
//         {
//             if (!handle_open(&myfield))
//             {
//                 myfield.gameover = true;
// #ifndef _PRETTY
//                 clrscrn();
// #endif // _PRETTY
//                 open_field(&myfield);
//                 print_field(&myfield);
//                 printf("Game Over!\nJe opende een bom!\n");
//                 write_save(myfield.size, standbombper, 0, seed, masks);
//                 break;
//             }

//             goto evaluation;
//         }
//         // flagging
//         if (input == 102)
//         {
//             Cell *curcell = myfield.cells[myfield.carety][myfield.caretx];
//             flag_cell(curcell);

//             goto evaluation;
//         }

//         // esc code
//         if (input == 27 || input == 113)
//         {
//             field_masks(&myfield, &masks);
//             write_save(myfield.size, standbombper, 1, seed, masks);
//             break;
//         }

//         // arrow-keys
//         if (arrow)
//         {
//             handle_arrows(&myfield, arrow);
//         }
//     evaluation:
//         // evaluate the field at the end of every
//         if (eval_field(&myfield))
//         {
// #ifndef _PRETTY
//             clrscrn();
// #endif // _PRETTY
//             open_field(&myfield);
//             print_field(&myfield);
//             printf("Je hebt gewonnen!\n");
//             write_save(myfield.size, standbombper, 0, seed, masks);
//             break;
//         }
//     }

//     // free the cells
//     for (uint32_t y = 0; y < myfield.size; y++)
//     {
//         for (uint32_t x = 0; x < myfield.size; x++)
//         {
//             free(myfield.cells[y][x]);
//         }
//         free(myfield.cells[y]);
//     }
//     free(myfield.cells);

//     // free the masks
//     for (uint32_t i = 0; i < myfield.size; i++)
//     {
//         free(masks[i]);
//     }
//     free(masks);

//     return EXIT_SUCCESS;
// }

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
    if (curcell->isbomb && curcell->isopened)
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