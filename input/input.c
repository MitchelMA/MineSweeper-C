#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include "input.h"

int get_arrow_keys(int *val)
{
    *val = 0;
    int ch = _getch();

    if (ch == 0 || ch == 0xE0)
    {
        int val = _getch();
        fflush(stdin);
        return val;
    }
    *val = ch;
    fflush(stdin);
    return 0;
}

void clrscrn(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif // _WIN32
}