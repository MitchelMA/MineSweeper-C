#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include "./input.h"

int get_arrow_keys(int *val)
{
    if (val != NULL)
        *val = 0;
    int ch = _getch();

    if (ch == 0 || ch == 0xE0)
    {
        int arr = _getch();
        fflush(stdin);
        return arr;
    }
    if (val != NULL)
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