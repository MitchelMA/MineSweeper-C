#include "saveio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int read_save(int *fieldsize, int *bombper, int ***masks)
{
    if (fieldsize == NULL || bombper == NULL || masks == NULL)
    {
        return 0;
    }
    // get a pointer to the file
    FILE *fp = fopen("save", "r");
    if (fp == NULL)
    {
        return 0;
    }
    fscanf(fp, "%d %d", fieldsize, bombper);
    printf("Fieldsize: %d, bomb-percentage: %d\n", *fieldsize, *bombper);

    bool hassave = false;
    fscanf(fp, "%d", &hassave);
    printf("hassave: %d\n", hassave);

    // allocate the memory for the array
    int **arr = malloc(sizeof(int *) * *fieldsize);
    for (int y = 0; y < *fieldsize; y++)
    {
        arr[y] = malloc(sizeof(int) * *fieldsize);
        for (int x = 0; x < *fieldsize; x++)
        {
            fscanf(fp, "%d", &arr[y][x]);
        }
    }

    *masks = arr;

    fclose(fp);
    return 1;
}

int write_save(int **masks)
{
    return 0;
}