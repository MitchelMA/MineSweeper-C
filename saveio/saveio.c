#include "saveio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int bin_write(const Field *field, int savefield)
{
    if (field == NULL)
    {
        return 0;
    }

    FILE *fp = fopen("save.bin", "wb");
    if (fp == NULL)
    {
        return 0;
    }

    char buff[256];
    sprintf(buff, "%zu %d ", field->size, field->bombper);
    if (fputs(buff, fp) == EOF)
    {
        fclose(fp);
        return 0;
    }
    if (savefield && (field->cells == NULL || field->seed == 0))
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

    sprintf(buff, " %d ", field->seed);
    if (fputs(buff, fp) == EOF)
    {
        fclose(fp);
        return 0;
    }

    Cell cells[field->size * field->size];
    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            size_t index = y * field->size + x;
            cells[index] = field->cells[y][x];
            cells[index].status ^= field->seed;
        }
    }
    size_t written = fwrite(cells, sizeof(Cell), sizeof(cells) / sizeof(cells[0]), fp);
    if (written != field->size * field->size)
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int bin_read(Field *field)
{
    if (field == NULL)
    {
        return 0;
    }

    FILE *fp = fopen("save.bin", "rb");
    if (fp == NULL)
    {
        return 0;
    }

    int hassave = 0;
    if (fscanf(fp, "%zu %d %d", &field->size, &field->bombper, &hassave) == EOF)
    {
        fclose(fp);
        return 0;
    }

    if (!hassave)
    {
        fclose(fp);
        return 2;
    }

    if (fscanf(fp, "%d  ", &field->seed) == EOF)
    {
        fclose(fp);
        return 0;
    }

    Cell cells[field->size * field->size];
    size_t read = fread(cells, sizeof(Cell), sizeof(cells) / sizeof(cells[0]), fp);
    if (read != field->size * field->size)
    {
        fclose(fp);
        return 0;
    }

    // allocate memory for the field.cells
    field->cells = malloc(sizeof(Cell *) * field->size);
    if (field->cells == NULL)
    {
        fclose(fp);
        return 0;
    }

    for (size_t y = 0; y < field->size; y++)
    {
        field->cells[y] = malloc(sizeof(Cell) * field->size);
        if (field->cells[y] == NULL)
        {
            fclose(fp);
            return 0;
        }
        for (size_t x = 0; x < field->size; x++)
        {
            size_t index = y * field->size + x;
            field->cells[y][x] = cells[index];
            field->cells[y][x].status ^= field->seed;
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