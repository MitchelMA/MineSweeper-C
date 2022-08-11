#include "saveio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int read_save(int *fieldsize, int *bombper, unsigned int *seed, int ***out_masks)
{
    if (fieldsize == NULL || bombper == NULL || out_masks == NULL)
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

    bool hassave = false;
    fscanf(fp, "%d", &hassave);

    if (!hassave)
    {
        fclose(fp);
        *out_masks = NULL;
        return 2;
    }

    // read the seed
    fscanf(fp, "%u", seed);

    // allocate the memory for the array
    int **arr = malloc(sizeof(int *) * *fieldsize);
    if (arr == NULL)
    {
        fclose(fp);
        return 0;
    }
    for (int y = 0; y < *fieldsize; y++)
    {
        arr[y] = malloc(sizeof(int) * *fieldsize);
        if (arr[y] == NULL)
        {
            fclose(fp);
            return 0;
        }
        for (int x = 0; x < *fieldsize; x++)
        {
            if (fscanf(fp, "%d", &arr[y][x]) == -1)
            {
                fclose(fp);
                return 0;
            }
        }
    }

    *out_masks = arr;

    fclose(fp);
    return 1;
}

int write_save(int fieldsize, int bombper, int savefield, int seed, int **masks)
{
    FILE *fp = fopen("save", "w");
    if (fp == NULL)
    {
        return 0;
    }

    char buff[256];
    sprintf(buff, "%d %d ", fieldsize, bombper);
    if (fputs(buff, fp) == EOF)
    {
        fclose(fp);
        return 0;
    }

    if (savefield && (masks == NULL || seed == 0))
    {
        fputc(0 + '0', fp);
        fclose(fp);
        return 0;
    }

    if (fputc(savefield + '0', fp) == EOF)
    {
        fclose(fp);
        return 0;
    }

    if (!savefield)
    {
        fclose(fp);
        return 2;
    }

    sprintf(buff, " %d ", seed);
    if (fputs(buff, fp) == EOF)
    {
        fclose(fp);
        return 0;
    }

    for (int y = 0; y < fieldsize; y++)
    {
        for (int x = 0; x < fieldsize; x++)
        {
            sprintf(buff, "%d ", masks[y][x]);
            if (fputs(buff, fp) == EOF)
            {
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    return 1;
}

int clear_save(void)
{
    fclose(fopen("save", "w"));
    return 1;
}