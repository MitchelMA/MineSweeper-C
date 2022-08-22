#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include "field/field.h"
#include "input/input.h"
#include "saveio/saveio.h"

static Field myfield;

int handle_open(Field *field);
void handle_arrows(Field *field, int arrow);
void handle_crash(int signo);

int main(int argc, char *argv[])
{
    // setup for detecting a segfault
    signal(SIGSEGV, handle_crash);

    if (argc == 1)
    {
        if (!bin_read(&myfield))
        {
            fprintf(stderr, "Er ging iets mis bij het uitlezen van het opslag-bestand\n");
            get_arrow_keys(NULL);
#ifdef _PRETTY
            printf("\x1b[A");
            for (int i = 0; i < 60; i++)
            {
                printf(" ");
            }
            printf("\x1b[G");
#endif // _PRETTY
        }
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
        fprintf(stderr, "Het lukte niet om het speelveld te initializeren!\n");

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
        printf("\x1b[%zuA", myfield.size);
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
                if (!bin_write(&myfield, 0))
                {
                    fprintf(stderr, "Het lukte helaas niet om het speelveld op te slaan\n");
                }
                break;
            }

            goto evaluation;
        }
        // flagging
        if (input == 102)
        {
            flag_cell(&myfield.cells[myfield.carety][myfield.caretx]);

            goto evaluation;
        }

        // esc code
        if (input == 27 || input == 113)
        {
#ifdef _PRETTY
            printf("\x1b[%zuB", myfield.size);
#endif // _PRETTY
            printf("Bedankt voor het spelen!");
            if (!bin_write(&myfield, 1))
            {
                fprintf(stderr, "Het lukte helaas niet om het speelveld op te slaan\n");
            }
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
            if (!bin_write(&myfield, 0))
            {
                fprintf(stderr, "Het lukte helaas niet om het speelveld op te slaan\n");
            }
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
        else
            field->carety = field->size - 1;
        break;
    }

    // right
    case 'M':
    {
        if (field->caretx < field->size - 1)
            field->caretx++;
        else
            field->caretx = 0;
        break;
    }

    // down
    case 'P':
    {
        if (field->carety < field->size - 1)
            field->carety++;
        else
            field->carety = 0;
        break;
    }

    // left
    case 'K':
    {
        if (field->caretx > 0)
            field->caretx--;
        else
            field->caretx = field->size - 1;
        break;
    }
    }
}

void handle_crash(int signo)
{
    // segfault signal
    if (signo == SIGSEGV)
    {
        fprintf(stderr, "Segfault..., poging tot opslaan van speelveld..\n");
        if (!bin_write(&myfield, !myfield.gameover))
        {
            fprintf(stderr, "Het lukte niet om het speelveld op te slaan\n");
        }
        else
        {
            printf("Speelveld met succes opgeslagen\n");
        }
    }
}