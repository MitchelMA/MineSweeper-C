#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "field/field.h"
#include "input/input.h"
#include "saveio/saveio.h"

int standfieldsize = 30;
int standbombper = 10;

// int write_save();
// int read_save();

int main(int argc, char *argv[])
{
    int **masks;
    // read the save
    int rstatus = read_save(&standfieldsize, &standbombper, &masks);
    if (!rstatus)
    {
        printf("Er ging iets mis bij het uitlezen van opslag-data!\n");
        return EXIT_FAILURE;
    }
    for (int y = 0; y < standfieldsize; y++)
    {
        for (int x = 0; x < standfieldsize; x++)
        {
            printf("%d ", masks[y][x]);
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}

int main2(int argc, char *argv[])
{
    int **masks;
    read_save(&standfieldsize, &standbombper, &masks);
    if (argc > 1)
    {
        argv++;
        standfieldsize = atoi(*argv);
    }
    if (argc > 2)
    {
        argv++;
        standbombper = atoi(*argv);
    }

    Field myfield;
    if (!init_field(&myfield, standfieldsize, standbombper))
    {
        printf("Het lukte niet om het speelveld te initializeren\n");
        return EXIT_FAILURE;
    }

    write_save(masks);

    int input = 0;

    while (input != 27)
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

// int write_save()
// {
//     FILE *f = fopen("save", "w");
//     char format[255] = {0};
//     sprintf(format, "%d %d", standfieldsize, standbombper);
//     int result = fputs(format, f);

//     fclose(f);
//     return result;
// }

// int read_save()
// {
//     FILE *f = fopen("save", "r");
//     if (f == NULL)
//     {
//         return 0;
//     }
//     char buff[255];

//     int result = fscanf(f, "%s", buff);
//     standfieldsize = atoi(buff);
//     result = fscanf(f, "%s", buff);
//     standbombper = atoi(buff);

//     fclose(f);

//     return result;
// }