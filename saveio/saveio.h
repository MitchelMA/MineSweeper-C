#ifndef __SAVEIO_H__
#define __SAVEIO_H__
#include <stdint.h>

int read_save(size_t *fieldsize, int *bombper, uint32_t *seed, uint32_t ***out_masks);
int write_save(size_t fieldsize, int bombper, int savefield, uint32_t seed, uint32_t **masks);
int clear_save(void);

#endif // __SAVEIO_H__