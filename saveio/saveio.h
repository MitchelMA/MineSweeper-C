#ifndef __SAVEIO_H__
#define __SAVEIO_H__
#include "..\field\field.h"
#include <stdint.h>

int read_save(Field *field);
int write_save(const Field *field, int savefield);
int clear_save(void);

#endif // __SAVEIO_H__