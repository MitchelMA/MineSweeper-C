#include <stdio.h>
#include <stdlib.h>
#include "field/field.h"
#include "input/input.h"

int main(void)
{
    Field myfield;
    init_field(&myfield, 40);

    int input = 0;

    while (input != 27)
    {

        // cursor movement
        print_field(&myfield);
        printf("\033[%dA", FIELD_SIZE);
        int arrow = get_arrow_keys(&input);

        // enter press
        if (input == 13)
        {
            continue;
        }
        // flagging
        if (input == 102)
        {
            continue;
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
                if (myfield.caretx < FIELD_SIZE - 1)
                    myfield.caretx++;
                break;
            }

            // down
            case 'P':
            {
                if (myfield.carety < FIELD_SIZE - 1)
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
            default:
                printf("%c, %d", arrow, arrow);
            }
        }
    }

    return 0;
}