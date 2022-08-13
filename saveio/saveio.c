#include "saveio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int read_save(size_t *fieldsize, int *bombper, uint32_t *seed, uint32_t ***out_masks)
{
    if (fieldsize == NULL || bombper == NULL || seed == NULL || out_masks == NULL)
    {
        return 0;
    }
    // get a pointer to the file
    FILE *fp = fopen("save", "r");
    if (fp == NULL)
    {
        return 0;
    }
    int hassave = false;
    if (fscanf(fp, "%zu %d %d", fieldsize, bombper, &hassave) == EOF)
    {
        fclose(fp);
        return 0;
    }

    if (!hassave)
    {
        fclose(fp);
        *out_masks = NULL;
        return 2;
    }

    // read the seed
    if (fscanf(fp, "%zd", seed) == EOF)
    {
        fclose(fp);
        return 0;
    }

    // allocate the memory for the array
    uint32_t **arr = malloc(sizeof(int *) * *fieldsize);
    if (arr == NULL)
    {
        fclose(fp);
        return 0;
    }
    for (size_t y = 0; y < *fieldsize; y++)
    {
        arr[y] = malloc(sizeof(int) * *fieldsize);
        if (arr[y] == NULL)
        {
            fclose(fp);
            return 0;
        }
        for (size_t x = 0; x < *fieldsize; x++)
        {
            if (fscanf(fp, "%X", &arr[y][x]) == EOF)
            {
                fclose(fp);
                return 0;
            }
            arr[y][x] ^= *seed;
        }
    }

    *out_masks = arr;

    fclose(fp);
    return 1;
}

int write_save(size_t fieldsize, int bombper, int savefield, uint32_t seed, uint32_t **masks)
{
    FILE *fp = fopen("save", "w");
    if (fp == NULL)
    {
        return 0;
    }

    char buff[256];
    sprintf(buff, "%zd %d ", fieldsize, bombper);
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

    for (size_t y = 0; y < fieldsize; y++)
    {
        for (size_t x = 0; x < fieldsize; x++)
        {
            sprintf(buff, "%X ", masks[y][x] ^ seed);
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