#ifndef __SAVEIO_H__
#define __SAVEIO_H__
#include "..\field\field.h"
#include <stdint.h>

int bin_read(Field *field);
int bin_write(const Field *field, int savefield);
int clear_save(void);

#endif // __SAVEIO_H__