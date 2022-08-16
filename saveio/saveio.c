#include "saveio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char _cell_hex_;

_cell_hex_ cto_ch_(const Cell *cell);
void htoc(const _cell_hex_ _hex_, Cell *out);

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
    sprintf(buff, "%zu %u ", field->size, field->bombper);
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

    sprintf(buff, " %u", field->seed);
    if (fputs(buff, fp) == EOF)
    {
        fclose(fp);
        return 0;
    }

    _cell_hex_ hexs[field->size * field->size];
    for (size_t y = 0; y < field->size; y++)
    {
        for (size_t x = 0; x < field->size; x++)
        {
            size_t index = y * field->size + x;
            hexs[index] = cto_ch_(&field->cells[y][x]);
        }
    }

    size_t written = fwrite(hexs, sizeof(_cell_hex_), field->size * field->size, fp);
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
    if (fscanf(fp, "%zu %u %d", &field->size, &field->bombper, &hassave) == EOF)
    {
        fclose(fp);
        return 0;
    }

    if (!hassave)
    {
        fclose(fp);
        return 2;
    }

    if (fscanf(fp, "%u", &field->seed) == EOF)
    {
        fclose(fp);
        return 0;
    }

    _cell_hex_ hexs[field->size * field->size];
    size_t read = fread(hexs, sizeof(_cell_hex_), field->size * field->size, fp);
    if (read != field->size * field->size)
    {
        printf("Er ging iets mis bij het uitlezen\n");
        printf("kon maar %d van de %d elementen uitlezen\n", read, field->size * field->size);
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
            htoc(hexs[index], &field->cells[y][x]);
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

#define NEIGHBOUR_VALUE 8
/*
    `Cell` to `_cell_hex_`
    A neighbour counts as 8.
    After calculating the correct value corresponding to the neighbour count,
    the status can just be added as a value.

    This will result in in the size of a character,
    that than can get pushed to the .bin file as a single byte.
    Instead of the 1 bytes + 3 bytes padding before.
    A 75% decrease in file-size.

    Thus
    a `Cell` with a status-value of 6 (is a bomb and is flagged),
    and a neighbour-count of 2, will result in a decimal value of 22.
    Or a hexadecimal value of 16.
*/
_cell_hex_ cto_ch_(const Cell *cell)
{
    const int nmult = 8;
    _cell_hex_ hex;
    hex = NEIGHBOUR_VALUE * cell->bombneighbours;
    hex += cell->status;
    return hex;
}

/*
    `_cell_hex_` to `Cell`
    By casting the` _hex_` to an unsigned integer,
    and then dividing it by the neighbour-value, will calculate the neighbour-count.
    The status can get calculated the same way, except that the `rest` value is needed after dividing.

    Thus a hexadecimal value of 16 (decimal value of 22) will result in a `Cell` with a status-value of 6 (bomb and flagged),
    and a neighbour-count of 2.
*/
void htoc(const _cell_hex_ _hex_, Cell *out)
{
    const int nmult = 8;
    int neighbours = (unsigned int)_hex_ / NEIGHBOUR_VALUE;
    int status = (unsigned int)_hex_ % NEIGHBOUR_VALUE;
    out->bombneighbours = neighbours;
    out->status = status;
}